#include "listenerhandler.h"
#include "rabbitmq/rabbitmqclient.h"

namespace Runner {

ListenerHandler::ListenerHandler(const QSharedPointer<RabbitUtils::QueueListener> &listener)
    : _listener(listener)
{
    if (!_listener)
        throw std::runtime_error("unable to create queue listener");

    initProcessFromBuffer();

    connect(_listener.data(), &RabbitUtils::QueueListener::readyChanged,
            this, &ListenerHandler::handleReadyChanged);
    connect(_listener.data(), &RabbitUtils::QueueListener::messageReceived,
            this, &ListenerHandler::sigMessageReceived);
}

void ListenerHandler::initProcessFromBuffer()
{
    const auto messages = _listener->messages();
    for (auto it = messages.cbegin(); it != messages.cend(); ++it)
        emit sigMessageReceived(it.value().message, it.key(), it.value().priority);
}

void ListenerHandler::confirmMessageTag(uint64_t deliveryTag)
{
    _listener->confirmReceiving(deliveryTag);
}

void ListenerHandler::handleReadyChanged(bool ready)
{
    if (!ready)
        emit sigConnectionBroken();
}

} // namespace Runner
