#pragma once

#include <QWeakPointer>

#include "rabbitmq_global.h"

namespace AMQP {
class Channel;
}

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

class RABBIT_MQ_EXPORT QueueSender : public QObject
{
    Q_OBJECT
    friend class RabbitMQClient;

public:
    QString queueName() const;

    bool isDurable() const;
    bool isReady() const;

    bool publish(const QByteArray& message, uint32_t ttl_s, uint8_t priority, bool isPersistent);

signals:
    void ready();
    void readyChanged(bool isReady);

private:
    QueueSender(const QString& queueName,
                bool isDurable,
                QWeakPointer<AMQP::Channel> channel,
                QObject* parent = nullptr);

    void setIsReady(bool isReady = true);
    void resetChannel(QWeakPointer<AMQP::Channel> channel);

private:
    QString _queueName;

    bool _isDurable;
    bool _isReady = false;

    QWeakPointer<AMQP::Channel> _channel;
};

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils
