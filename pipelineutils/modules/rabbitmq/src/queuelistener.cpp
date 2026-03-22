#include "rabbitmq/queuelistener.h"

#include "amqpcpp.h"

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

QueueListener::CachedMessage::CachedMessage(QByteArray msg, uint8_t prio)
    : message(std::move(msg))
    , priority(prio)
{
}

QueueListener::QueueListener(const QString& queueName,
                             bool isAutoAcknowledge,
                             QWeakPointer<AMQP::Channel> channel,
                             QObject* parent)
    : QObject(parent)
    , _queueName(queueName)
    , _isAutoAcknowledge(isAutoAcknowledge)
    , _channel(channel)
{
}

QString QueueListener::queueName() const
{
    return _queueName;
}

bool QueueListener::isAutoAcknowledge() const
{
    return _isAutoAcknowledge;
}

bool QueueListener::isReady() const
{
    return _isReady;
}

void QueueListener::setIsReady(bool isReady)
{
    if (_isReady != isReady)
        emit readyChanged(isReady);

    if ((_isReady = isReady))
        emit ready();
    else
        _messages.clear();
}

QHash<uint64_t, QueueListener::CachedMessage> QueueListener::messages() const
{
    return _messages;
}

bool QueueListener::confirmReceiving(const uint64_t& deliveryTag)
{
    auto it = _messages.find(deliveryTag);
    if (!_isReady || _isAutoAcknowledge || it == _messages.end())
        return false;

    if (auto channel = _channel.toStrongRef())
        if (channel->ack(deliveryTag))
            return _messages.erase(it), true;
        else
            return false;
    else
        return false;
}

void QueueListener::resetChannel(QWeakPointer<AMQP::Channel> channel)
{
    _channel = channel;
    setIsReady(false);
}

void QueueListener::addMessage(const AMQP::Message* msg, const uint64_t& deliveryTag)
{
    const auto message = QByteArray(msg->body(), msg->bodySize());
    if (_isAutoAcknowledge)
    {
        if (auto channel = _channel.toStrongRef())
            channel->ack(deliveryTag);
    }
    else
    {
        _messages.insert(deliveryTag, CachedMessage(message, msg->priority()));
        emit messagesUpdated();
    }
    emit messageReceived(message, msg->priority(), deliveryTag);
}

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils
