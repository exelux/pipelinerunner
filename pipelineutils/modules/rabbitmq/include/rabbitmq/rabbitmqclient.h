#pragma once

#include <QHostAddress>

#include "rabbitmq_global.h"

namespace AMQP {
class Connection;
class Channel;
} // namespace AMQP

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

class QueueSender;
class QueueListener;
class ConnectionHandler;

struct ConnectionInfoStruct
{
    ConnectionInfoStruct(const QHostAddress& rabbitServerIP = QHostAddress("127.0.0.1"),
                         const uint16_t& rabbitServerPort = 5672,
                         const QString& log = "guest",
                         const QString& pswd = "guest")
        : address(rabbitServerIP)
        , port(rabbitServerPort)
        , login(log)
        , password(pswd)
    {
    }

    QHostAddress address;
    uint16_t port;
    QString login;
    QString password;
};

class RABBIT_MQ_EXPORT RabbitMQClient : public QObject
{
    Q_OBJECT
public:
    RabbitMQClient(const QHostAddress& rabbitServerIP = QHostAddress("127.0.0.1"),
                   const uint16_t& rabbitServerPort = 5672,
                   const QString& login = "guest",
                   const QString& password = "guest",
                   QObject* parent = nullptr);
    RabbitMQClient(const ConnectionInfoStruct& serverInfo, QObject* parent = nullptr);
    ~RabbitMQClient() override;

    bool isConnected() const;
    bool isReady() const;

    QSharedPointer<QueueSender> createQueueSender(const QString& queueName, bool isDurable);
    QSharedPointer<QueueListener> createQueueListener(const QString& queueName,
                                                      bool isAutoAcknowledge);
    QSharedPointer<QueueSender> getQueueSender(const QString& queueName) const;
    QSharedPointer<QueueListener> getQueueListener(const QString& queueName) const;
    ConnectionInfoStruct getServerConnectionInfo() const;

public slots:
    void start();

signals:
    void connectionChanged(bool isConnected);
    void readyChanged(bool isReady);

private slots:
    void handleConnectionChanged(bool isConnected);
    void handleReadyChanged(bool isReady);

private:
    void registerQueueSender(QSharedPointer<QueueSender> queueSender);
    void registerQueueListener(QSharedPointer<QueueListener> queueListener);

private:
    static const uint16_t prefetchCount = 1;
    static const uint8_t maxPriority = 3;

private:
    ConnectionInfoStruct _rabbitServerInfo;

    QHash<QString, QSharedPointer<QueueSender>> _senders;
    QHash<QString, QSharedPointer<QueueListener>> _listeners;

    QSharedPointer<ConnectionHandler> _handler;
    QSharedPointer<AMQP::Connection> _connection;
    QSharedPointer<AMQP::Channel> _channel;
};

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils
