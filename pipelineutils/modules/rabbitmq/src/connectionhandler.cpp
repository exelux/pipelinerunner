#include "connectionhandler.h"

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

ConnectionHandler::ConnectionHandler(const QHostAddress &host, uint16_t port, QObject *parent)
    : QObject(parent)
    , AMQP::ConnectionHandler()
    , _host(host)
    , _port(port)
    , _inputBuffer(IN_BUFFER_SIZE)
    , _outBuffer(OUT_BUFFER_SIZE)
    , _tmpBuffer(TEMP_BUFFER_SIZE)
{
}

void ConnectionHandler::start()
{
    connect(&_socket, &QTcpSocket::connected,
            this, &ConnectionHandler::handleConnected);
    connect(&_socket, &QTcpSocket::disconnected,
            this, &ConnectionHandler::handleDisconnected);
    connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, &ConnectionHandler::handleSocketError);
    connect(&_socket, &QTcpSocket::readyRead,
            this, &ConnectionHandler::handleReadyRead);
    connect(&_reconnectTimer, &QTimer::timeout,
            this, &ConnectionHandler::tryConnect);

    _reconnectTimer.setSingleShot(true);
    _reconnectTimer.setInterval(1000);
    _socket.setSocketOption(QTcpSocket::SocketOption::KeepAliveOption, true);
    tryConnect();
}

ConnectionHandler::~ConnectionHandler()
{
    disconnect(&_socket, &QTcpSocket::connected, this, &ConnectionHandler::handleConnected);
    disconnect(&_socket, &QTcpSocket::disconnected, this, &ConnectionHandler::handleDisconnected);
    disconnect(&_socket,
               QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
               this,
               &ConnectionHandler::handleSocketError);
    disconnect(&_socket, &QTcpSocket::readyRead, this, &ConnectionHandler::handleReadyRead);
    _socket.close();
}

void ConnectionHandler::tryConnect()
{
    if (!isConnected() && _socket.state() != QAbstractSocket::SocketState::ConnectingState)
        _socket.connectToHost(_host, _port);
}

bool ConnectionHandler::isConnected() const
{
    return _isConnected;
}

bool ConnectionHandler::isReady() const
{
    return _isReady;
}

void ConnectionHandler::handleConnected()
{
    setConnected(true);
}

void ConnectionHandler::handleDisconnected()
{
    setConnected(false);
    _reconnectTimer.start();
}

void ConnectionHandler::handleSocketError()
{
    setConnected(false);
    _reconnectTimer.start();
}

void ConnectionHandler::setConnected(bool connected)
{
    if (_isConnected != connected)
        emit connectionChanged(connected);

    if (!(_isConnected = connected))
        setReady(false);
}

void ConnectionHandler::setReady(bool ready)
{
    if (_isReady != ready)
        emit readyChanged(ready);
    _isReady = ready;
}

void ConnectionHandler::handleReadyRead()
{
    while (_socket.bytesAvailable() > 0)
    {
        const auto bytesAvailable = _socket.bytesAvailable();
        const auto bytesToRead = std::min<qint64>(bytesAvailable, TEMP_BUFFER_SIZE);

        _socket.read(_tmpBuffer.data(), bytesToRead);

        const auto availableSize = _inputBuffer.availableSize();
        // check if input buffer if full, if yes increase size only once,
        // in second time disconnect from server, message will not been lost
        // because server must receive confirmation that sent after parsing
        if (availableSize < bytesToRead)
        {
            if (!_isInputBufferResized)
            {
                _inputBuffer.resize(IN_BUFFER_SIZE + bytesToRead - availableSize);
                _isInputBufferResized = true;
            }
            else
            {
                close();
                return setConnected(false);
            }
        }

        _inputBuffer.write(_tmpBuffer.data(), bytesToRead);
        if (_connection && _inputBuffer.dataSize())
        {
            const auto count = _connection->parse(_inputBuffer.data(), _inputBuffer.dataSize());

            _inputBuffer.shiftLeft(count);
            if (_isInputBufferResized && count)
            {
                const auto notHandledBufferSize = _inputBuffer.size() - count;
                if (notHandledBufferSize > IN_BUFFER_SIZE)
                {
                    _inputBuffer.resize(notHandledBufferSize);
                }
                else
                {
                    _inputBuffer.resize(IN_BUFFER_SIZE);
                    _isInputBufferResized = false;
                }
            }
        }
    }
}

void ConnectionHandler::onData(AMQP::Connection *connection, const char *data, size_t size)
{
    checkConnection(connection);
    for (size_t written = 0; written < size;)
    {
        written += _outBuffer.write(data + written, size - written);
        sendDataFromBuffer();
    }
}

void ConnectionHandler::onReady(AMQP::Connection * /*connection*/)
{
    setReady(true);
}

void ConnectionHandler::onError(AMQP::Connection *connection, const char *message)
{
    std::cerr << "AMQP error " << message
              << (connection == _connection ? " current connection" : " other connection")
              << std::endl;
}

void ConnectionHandler::onClosed(AMQP::Connection * /*connection*/)
{
    std::cout << "AMQP closed connection" << std::endl;
    setConnected(false);
}

void ConnectionHandler::sendDataFromBuffer()
{
    if (_outBuffer.dataSize())
    {
        const auto bytesWritten = _socket.write(_outBuffer.data(), _outBuffer.dataSize());
        _outBuffer.shiftLeft(bytesWritten);
    }
}

void ConnectionHandler::checkConnection(AMQP::Connection *connection)
{
    if (!_connection)
    {
        _connection = connection;
        return;
    }

    if (_connection != connection)
    {
        _outBuffer.drain();
        _inputBuffer.drain();
        _connection = connection;
    }
}

void ConnectionHandler::close()
{
    sendDataFromBuffer();
    if (_connection)
        _connection->close();
}

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils
