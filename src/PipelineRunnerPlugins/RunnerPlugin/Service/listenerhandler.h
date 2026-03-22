#pragma once

#include <QObject>

#include "rabbitmq/queuelistener.h"

namespace Runner {

namespace RabbitUtils = SatDataPipelineUtils::RabbitMQModule;

class ListenerHandler : public QObject
{
    Q_OBJECT

public:
    ListenerHandler(const QSharedPointer<RabbitUtils::QueueListener> &client);

    void confirmMessageTag(uint64_t deliveryTag);

signals:

    void sigMessageReceived(const QByteArray &message, uint8_t priority, uint64_t deliveryTag);
    void sigConnectionBroken();

private slots:
    void handleReadyChanged(bool ready);

private:
    void initProcessFromBuffer();

private:
    QSharedPointer<RabbitUtils::QueueListener> _listener;
};

} // namespace Runner
