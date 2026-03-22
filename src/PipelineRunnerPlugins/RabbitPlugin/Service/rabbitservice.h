#pragma once

#include <QSharedPointer>
#include <memory>
#include <mutex>

#include "Common/Services/irabbitservice.h"
#include "Common/Structs/config.h"
#include "rabbitmq/rabbitmqclient.h"

namespace Rabbit {

namespace RabbitUtils = SatDataPipelineUtils::RabbitMQModule;

class RabbitService : public Common::IRabbitService
{
    Q_OBJECT

public:
    explicit RabbitService(QObject *parent = nullptr);
    void start();

public:
    QSharedPointer<RabbitUtils::QueueSender> pubQueueSender() final;
    QSharedPointer<RabbitUtils::QueueListener> subQueueListener() final;

    bool isConnected() const final;
    bool isPubQueueCreated() const final;
    bool isSubQueueConnected() const final;

    Common::ConnectionInfo getServerConnectionInfo() const final;
    QString getPubQueueName() const final;
    QString getSubQueueName() const final;

private:
    mutable std::mutex _sync;
    std::unique_ptr<RabbitUtils::RabbitMQClient> _client;
    Common::Config _config{};
};

} // namespace Rabbit
