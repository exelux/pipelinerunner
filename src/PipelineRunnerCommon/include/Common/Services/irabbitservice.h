#pragma once

#include <QHostAddress>
#include <Plt2/Core/Interface/IService>

#include "PipelineRunnerCommon_global.h"

namespace SatDataPipelineUtils {
namespace RabbitMQModule {
class QueueSender;
class QueueListener;
} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils

namespace Common {

namespace RabbitUtils = SatDataPipelineUtils::RabbitMQModule;

struct ConnectionInfo
{
    QHostAddress address;
    uint16_t port;
    QString login;
    QString password;
};

class COMMON_EXPORT IRabbitService : public Plt2::Core::IServiceT<IRabbitService>
{
    Q_OBJECT

public:
    explicit IRabbitService(QObject *parent = nullptr);

    virtual QSharedPointer<RabbitUtils::QueueSender> pubQueueSender() = 0;
    virtual QSharedPointer<RabbitUtils::QueueListener> subQueueListener() = 0;

    virtual bool isConnected() const = 0;
    virtual bool isPubQueueCreated() const = 0;
    virtual bool isSubQueueConnected() const = 0;

    virtual ConnectionInfo getServerConnectionInfo() const = 0;
    virtual QString getPubQueueName() const = 0;
    virtual QString getSubQueueName() const = 0;
};

} // namespace Common
