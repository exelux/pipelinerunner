#include "rabbitservice.h"
#include "Common/Services/iconfigservice.h"
#include "rabbitmq/queuelistener.h"
#include "rabbitmq/queuesender.h"

#include <QLoggingCategory>
#include <Plt2/Core/ServiceLocator>

Q_DECLARE_LOGGING_CATEGORY(rabbitplugin)

namespace Rabbit {

static const auto pub_is_durable_queue = true;
static const auto sub_queue_auto_acknowledge = false;

RabbitService::RabbitService(QObject *parent)
    : Common::IRabbitService(parent)
{
}

void RabbitService::start()
{
    const auto configService = Plt2::Core::ServiceLocator::service<Common::IConfigService>();
    if (!configService)
        throw std::runtime_error("unable to access config service");

    _config = configService->config();

    const auto startClient = [this]() {
        std::lock_guard<std::mutex> lock(_sync);
        _client = std::make_unique<RabbitUtils::RabbitMQClient>(_config.rabbitHost,
                                                                _config.rabbitPort,
                                                                _config.rabbitLogin,
                                                                _config.rabbitPassword);
        return _client->start(), _client->getServerConnectionInfo();
    };

    const auto info = startClient();
    qCInfo(rabbitplugin).nospace()
        << "rabbit client started on " << info.address.toString() << ":" << info.port;
}

Common::ConnectionInfo RabbitService::getServerConnectionInfo() const
{
    const auto convert = [](const RabbitUtils::ConnectionInfoStruct &rhs) {
        return Common::ConnectionInfo{rhs.address, rhs.port, rhs.login, rhs.password};
    };

    const auto connection_info = [this]() {
        std::lock_guard<std::mutex> lock(_sync);
        return _client ? _client->getServerConnectionInfo() : RabbitUtils::ConnectionInfoStruct();
    };

    return convert(connection_info());
}

QSharedPointer<RabbitUtils::QueueSender> RabbitService::pubQueueSender()
{
    std::lock_guard<std::mutex> lock(_sync);
    return _client ? _client->createQueueSender(_config.pubQueueName, pub_is_durable_queue)
                   : nullptr;
}

QSharedPointer<RabbitUtils::QueueListener> RabbitService::subQueueListener()
{
    std::lock_guard<std::mutex> lock(_sync);
    return _client ? _client->createQueueListener(_config.subQueueName, sub_queue_auto_acknowledge)
                   : nullptr;
}

bool RabbitService::isConnected() const
{
    std::lock_guard<std::mutex> lock(_sync);
    return _client && _client->isConnected() && _client->isReady();
}

bool RabbitService::isPubQueueCreated() const
{
    std::lock_guard<std::mutex> lock(_sync);
    const auto sender = _client ? _client->getQueueSender(_config.pubQueueName) : nullptr;
    return sender && sender->isReady();
}

bool RabbitService::isSubQueueConnected() const
{
    std::lock_guard<std::mutex> lock(_sync);
    const auto listener = _client ? _client->getQueueListener(_config.subQueueName) : nullptr;
    return listener && listener->isReady();
}

QString RabbitService::getPubQueueName() const
{
    return _config.pubQueueName;
}

QString RabbitService::getSubQueueName() const
{
    return _config.subQueueName;
}
} // namespace Rabbit
