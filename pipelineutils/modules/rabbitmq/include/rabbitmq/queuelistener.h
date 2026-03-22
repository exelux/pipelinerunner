#pragma once

#include <QHash>
#include <QWeakPointer>

#include "rabbitmq_global.h"

namespace AMQP {
class Message;
class Channel;
} // namespace AMQP

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

class RABBIT_MQ_EXPORT QueueListener : public QObject
{
    Q_OBJECT

    friend class RabbitMQClient;

    struct CachedMessage
    {
        QByteArray message;
        uint8_t priority;

        CachedMessage(QByteArray msg, uint8_t prio);
    };

public:
    QString queueName() const;

    bool isAutoAcknowledge() const;
    bool isReady() const;

    QHash<uint64_t /*deliveryTag*/, CachedMessage> messages() const;
    bool confirmReceiving(const uint64_t& deliveryTag);

signals:
    void ready();
    void readyChanged(bool isReady);
    void messageReceived(QByteArray message, uint8_t priority, uint64_t deliveryTag);
    void messagesUpdated();

private:
    QueueListener(const QString& queueName,
                  bool isAutoAcknowledge,
                  QWeakPointer<AMQP::Channel> channel,
                  QObject* parent = nullptr);

    void setIsReady(bool isReady = true);
    void resetChannel(QWeakPointer<AMQP::Channel> channel);
    void addMessage(const AMQP::Message* msg, const uint64_t& deliveryTag);

private:
    QString _queueName;

    bool _isAutoAcknowledge;
    bool _isReady = false;

    QWeakPointer<AMQP::Channel> _channel;
    QHash<uint64_t, CachedMessage> _messages;
};

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils
