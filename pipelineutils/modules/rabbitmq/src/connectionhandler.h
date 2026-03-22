#pragma once

#include <QHostAddress>
#include <QTcpSocket>
#include <QTimer>

#include "amqpcpp.h"
#include "buffer.h"

namespace SatDataPipelineUtils {
namespace RabbitMQModule {

class ConnectionHandler : public QObject, public AMQP::ConnectionHandler
{
    Q_OBJECT

public:
    ConnectionHandler(const QHostAddress &host, uint16_t port, QObject *parent = nullptr);
    ~ConnectionHandler() override;

    bool isConnected() const;
    bool isReady() const;

    void close();
    void start();

signals:
    void connectionChanged(bool isConnected);
    void readyChanged(bool isReady);

private:
    static constexpr const size_t IN_BUFFER_SIZE = 8 * 1024 * 1024; //8Mb buffer for data from server to parse
    static constexpr const size_t OUT_BUFFER_SIZE = 1 * 1024 * 1024; //1Mb buffer for data to server
    static constexpr const size_t TEMP_BUFFER_SIZE = 1 * 024 * 1024; //1Mb buffer for data from socket to input_buffer

private slots:
    void handleReadyRead();
    void handleConnected();
    void handleDisconnected();
    void handleSocketError();

private:
    ConnectionHandler(const ConnectionHandler &) = delete;
    ConnectionHandler &operator=(const ConnectionHandler &) = delete;

    void onData(AMQP::Connection *connection, const char *data, size_t size) override;
    void onReady(AMQP::Connection *connection) override;
    void onError(AMQP::Connection *connection, const char *message) override;
    void onClosed(AMQP::Connection *connection) override;

    void sendDataFromBuffer();
    void checkConnection(AMQP::Connection *connection);
    void setConnected(bool connected);
    void setReady(bool ready);
    void tryConnect();

private:
    QHostAddress _host;
    uint16_t _port;
    QTcpSocket _socket;
    bool _isConnected = false;
    bool _isReady = false;
    AMQP::Connection *_connection = nullptr;
    Buffer _inputBuffer;
    Buffer _outBuffer;
    std::vector<char> _tmpBuffer;
    QTimer _reconnectTimer;
    bool _isInputBufferResized = false;
};

} // namespace RabbitMQModule
} // namespace SatDataPipelineUtils
