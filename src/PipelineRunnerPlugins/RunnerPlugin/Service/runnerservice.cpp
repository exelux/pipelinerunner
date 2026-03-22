#include "runnerservice.h"
#include "Common/Services/iconverterservice.h"
#include "Common/Services/irabbitservice.h"
#include "Common/Structs/jobinfo.h"
#include "listenerhandler.h"
#include "rabbitmq/queuesender.h"

#include <QJsonDocument>
#include <QLoggingCategory>
#include <Plt2/Core/ServiceLocator>

Q_DECLARE_LOGGING_CATEGORY(runnerplugin)

namespace Runner {

static const auto pub_TTL_s = 0;
static const auto pub_is_persistent = true;
static const auto pub_priority = 0;

RunnerService::RunnerService(QObject *parent)
    : Common::IRunnerService(parent)
{
}

void RunnerService::start()
{
    checkJobSenderAndListener();
}

void RunnerService::handleJobMessage(const QByteArray &message,
                                     uint8_t priority,
                                     uint64_t deliveryTag)
{
    const auto process = [this](const QByteArray &message, uint8_t priority, uint64_t deliveryTag) {
        QJsonParseError error;
        const auto doc = QJsonDocument::fromJson(message, &error);

        if (error.error != QJsonParseError::NoError)
        {
            qCCritical(runnerplugin) << "unable to parse json:" << error.errorString();
            return false;
        }

        const Common::MessageInfo messageInfo(doc.object(), deliveryTag, priority);
        if (messageInfo.id.isNull())
        {
            qCCritical(runnerplugin) << "field 'message_id' was not found";
            return false;
        }

        auto foundTag = _jobsDeliveryTags.find(messageInfo.id);
        if (foundTag != _jobsDeliveryTags.end())
        {
            foundTag.value() = deliveryTag;
            return true;
        }

        const auto converter = Plt2::Core::ServiceLocator::service<Common::IConverterService>();
        if (!converter)
        {
            qCCritical(runnerplugin) << "converter service unavailable";
            return false;
        }

        try
        {
            const auto message = Common::messageFromJson(messageInfo.json);
            const auto handler = converter->registerJobMessage(message);
            if (!handler)
            {
                qCCritical(runnerplugin) << "invalid handle from converter";
                return false;
            }

            auto callback = std::bind(&RunnerService::handleJobFinished, this, handler.toWeakRef());
            connect(handler.data(), &Common::IJobHandle::finished, std::move(callback));

            if (!converter->processJobMessage(handler))
            {
                qCCritical(runnerplugin) << "error starting task from converter";
                return false;
            }

            _jobsDeliveryTags.insert(messageInfo.id, deliveryTag);
            _jobs.insert(messageInfo.id, handler);
        }
        catch (const std::exception &e)
        {
            qCCritical(runnerplugin) << e.what();
            return false;
        }

        return true;
    };

    if (!process(message, priority, deliveryTag))
        _listenerHandler->confirmMessageTag(deliveryTag);
}

void RunnerService::handleJobFinished(QSharedPointer<Common::IJobHandle> handle)
{
    if (!handle)
    {
        qCCritical(runnerplugin) << "failed to finish job, invalid handle";
        return;
    }

    const auto messageId = handle->message().messageId;

    auto found = _jobs.find(messageId);
    if (found == _jobs.end())
    {
        qCCritical(runnerplugin) << "failed to finish job, result received from unexpected handler";
        return;
    }

    auto foundTag = _jobsDeliveryTags.find(messageId);
    if (foundTag == _jobsDeliveryTags.end())
    {
        qCCritical(runnerplugin) << "failed to finish job, unknown delivery tag";
        return;
    }

    const auto result = handle->result();
    if (result.has_value())
    {
        const auto message = handle->message();

        Common::Message outMessage{};
        outMessage.messageId = QUuid::createUuid();
        outMessage.inputFile = result.value();

        sendResultMessage(Common::messageToByteArray(outMessage), pub_priority);
    }

    _listenerHandler->confirmMessageTag(foundTag.value());
    _jobs.erase(found);
    _jobsDeliveryTags.erase(foundTag);
}

void RunnerService::abortAllJobs()
{
    _jobs.clear();
}

void RunnerService::checkJobSenderAndListener()
{
    const auto rabbit = Plt2::Core::ServiceLocator::service<Common::IRabbitService>();
    if (!rabbit)
    {
        qCCritical(runnerplugin) << "rabbit service unavailable";
        return _listenerHandler.reset();
    }

    if (!rabbit->pubQueueSender())
    {
        qCCritical(runnerplugin) << "queue sender for tasks result not created";
        return _listenerHandler.reset();
    }
    else
    {
        qCInfo(runnerplugin) << "pub queue name:" << rabbit->getPubQueueName();
    }

    if (_listenerHandler)
        return;

    try
    {
        _listenerHandler = std::make_unique<ListenerHandler>(rabbit->subQueueListener());
        qCInfo(runnerplugin) << "sub queue name:" << rabbit->getSubQueueName();
    }
    catch (const std::exception &ex)
    {
        qCCritical(runnerplugin) << "failed to create sub queue listener";
        return;
    }

    connect(_listenerHandler.get(), &ListenerHandler::sigMessageReceived,
            this, &RunnerService::handleJobMessage);
    connect(_listenerHandler.get(), &ListenerHandler::sigConnectionBroken,
            this, &RunnerService::abortAllJobs);
}

void RunnerService::sendResultMessage(const QByteArray &data, uint8_t priority) const
{
    const auto rabbitService = Plt2::Core::ServiceLocator::service<Common::IRabbitService>();
    if (!rabbitService)
    {
        qCCritical(runnerplugin) << "rabbit service unavailable";
        return;
    }

    const auto pubQueueSender = rabbitService->pubQueueSender();
    if (!pubQueueSender)
    {
        qCCritical(runnerplugin) << "queue sender for tasks not created";
        return;
    }

    const auto published = pubQueueSender->publish(data, pub_TTL_s, priority, pub_is_persistent);
    if (!published)
    {
        qCCritical(runnerplugin) << "error sending message to result queue";
        return;
    }
}

} // namespace Runner
