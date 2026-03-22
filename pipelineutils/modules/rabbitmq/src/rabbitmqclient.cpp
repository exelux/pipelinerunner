#include "rabbitmq/rabbitmqclient.h"
#include "connectionhandler.h"
#include "rabbitmq/queuelistener.h"
#include "rabbitmq/queuesender.h"

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

RabbitMQClient::RabbitMQClient(const QHostAddress &address,
                               const uint16_t &port,
                               const QString &login,
                               const QString &password,
                               QObject *parent)
    : RabbitMQClient({address, port, login, password}, parent)
{
}

RabbitMQClient::RabbitMQClient(const ConnectionInfoStruct &serverInfo, QObject *parent)
    : QObject(parent)
    , _rabbitServerInfo(serverInfo)
{
}

RabbitMQClient::~RabbitMQClient()
{
    if (_handler)
        _handler->close();
}

void RabbitMQClient::start()
{
    _handler = QSharedPointer<ConnectionHandler>::create(_rabbitServerInfo.address,
                                                         _rabbitServerInfo.port);

    connect(_handler.data(), &ConnectionHandler::connectionChanged,
            this, &RabbitMQClient::handleConnectionChanged);
    connect(_handler.data(), &ConnectionHandler::readyChanged,
            this, &RabbitMQClient::handleReadyChanged);

    _handler->start();
}

void RabbitMQClient::handleConnectionChanged(bool isConnected)
{
    if (isConnected)
    {
        if (_connection)
            _connection->close();

        _connection.reset(
            new AMQP::Connection(_handler.data(),
                                 AMQP::Login(_rabbitServerInfo.login.toStdString(),
                                             _rabbitServerInfo.password.toStdString())));
        _channel.reset(new AMQP::Channel(_connection.data()));
        _channel->setQos(prefetchCount);

        for (const auto &sender : qAsConst(_senders))
            sender->resetChannel(_channel);
        for (const auto &listener : qAsConst(_listeners))
            listener->resetChannel(_channel);
    }
    emit connectionChanged(isConnected);
}

void RabbitMQClient::handleReadyChanged(bool isReady)
{
    if (isReady)
    {
        for (const auto &sender : qAsConst(_senders))
            registerQueueSender(sender);
        for (const auto &listener : qAsConst(_listeners))
            registerQueueListener(listener);
    }
    else
    {
        for (const auto &sender : qAsConst(_senders))
            sender->setIsReady(false);
        for (const auto &listener : qAsConst(_listeners))
            listener->setIsReady(false);
    }
    emit readyChanged(isReady);
}

bool RabbitMQClient::isConnected() const
{
    if (_handler)
        return _handler->isConnected();
    return false;
}

bool RabbitMQClient::isReady() const
{
    if (_handler)
        return _handler->isReady();
    return false;
}

QSharedPointer<QueueSender> RabbitMQClient::createQueueSender(const QString &queueName,
                                                              bool isDurable)
{
    if (auto queueSender = getQueueSender(queueName))
        return queueSender;

    const auto sender = QSharedPointer<QueueSender>(new QueueSender(queueName, isDurable, _channel));
    _senders.insert(queueName, sender);
    return registerQueueSender(sender), sender;
}

void RabbitMQClient::registerQueueSender(QSharedPointer<QueueSender> queueSender)
{
    const AMQP::QueueCallback callbackCreateQueue =
        [this](const std::string &name, int /*msg_count*/, int /*consumer_count*/) {
            if (auto queueSender = _senders.value(QString(name.data())))
                queueSender->setIsReady(true);
        };

    const AMQP::ErrorCallback errorCallback = [](const char *message) {
        qDebug() << "Error create queue: " << message;
    };

    AMQP::Table queue_args;
    queue_args["x-max-priority"] = maxPriority;

    if (_channel)
        _channel
            ->declareQueue(queueSender->queueName().toStdString(),
                           queueSender->isDurable() ? AMQP::durable : 0,
                           queue_args)
            .onSuccess(std::move(callbackCreateQueue))
            .onError(std::move(errorCallback));
}

void RabbitMQClient::registerQueueListener(QSharedPointer<QueueListener> queueListener)
{
    const AMQP::MessageCallback recvCallback =
        [this](const AMQP::Message &message, uint64_t deliveryTag, bool /*redelivered*/) {
            if (auto listener = _listeners.value(QString::fromStdString(message.routingkey())))
                listener->addMessage(&message, deliveryTag);
        };

    const AMQP::ConsumeCallback onSuccessCallback = std::bind(&QueueListener::setIsReady,
                                                              queueListener,
                                                              true);

    if (_channel)
        _channel->consume(queueListener->queueName().toStdString())
            .onReceived(std::move(recvCallback))
            .onSuccess(std::move(onSuccessCallback));
}

QSharedPointer<QueueListener> RabbitMQClient::createQueueListener(const QString &queueName,
                                                                  bool isAutoAcknowledge)
{
    if (auto queueListener = getQueueListener(queueName))
        return queueListener;

    const auto listener = QSharedPointer<QueueListener>(
        new QueueListener(queueName, isAutoAcknowledge, _channel));
    _listeners.insert(queueName, listener);
    return registerQueueListener(listener), listener;
}

QSharedPointer<QueueSender> RabbitMQClient::getQueueSender(const QString &queueName) const
{
    if (!_handler)
        return nullptr;

    return _senders.value(queueName);
}

QSharedPointer<QueueListener> RabbitMQClient::getQueueListener(const QString &queueName) const
{
    if (!_handler)
        return nullptr;

    return _listeners.value(queueName);
}

ConnectionInfoStruct RabbitMQClient::getServerConnectionInfo() const
{
    return _rabbitServerInfo;
}

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils
