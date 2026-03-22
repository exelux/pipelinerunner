#include "rabbitmq/queuesender.h"

#include "amqpcpp.h"

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

QueueSender::QueueSender(const QString& queueName,
                         bool isDurable,
                         QWeakPointer<AMQP::Channel> channel,
                         QObject* parent)
    : QObject(parent)
    , _queueName(queueName)
    , _isDurable(isDurable)
    , _channel(channel)
{
}

QString QueueSender::queueName() const
{
    return _queueName;
}

bool QueueSender::isDurable() const
{
    return _isDurable;
}

bool QueueSender::isReady() const
{
    return _isReady;
}

void QueueSender::setIsReady(bool isReady)
{
    if (_isReady != isReady)
        emit readyChanged(isReady);

    if ((_isReady = isReady))
        emit ready();
}

void QueueSender::resetChannel(QWeakPointer<AMQP::Channel> channel)
{
    _channel = channel;
    setIsReady(false);
}

bool QueueSender::publish(const QByteArray& message,
                          uint32_t ttl_s,
                          uint8_t priority,
                          bool isPersistent)
{
    if (!_isReady)
        return false;

    if (auto channel = _channel.toStrongRef())
    {
        AMQP::Envelope env(message.data());
        env.setPersistent(isPersistent);
        env.setPriority(priority);
        if (ttl_s)
            env.setExpiration(std::to_string(ttl_s * 1000));
        return channel->publish("", _queueName.toStdString(), env);
    }
    return false;
}

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils
