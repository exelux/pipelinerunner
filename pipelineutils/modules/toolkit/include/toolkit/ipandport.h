#ifndef IPANDPORT_H
#define IPANDPORT_H

#include <QHostAddress>
#include <QJsonValue>
#include "toolkit_global.h"

namespace Toolkit {

class TOOLKIT_EXPORT IpAndPort
{
public:
    IpAndPort()
        : _ip(QHostAddress::Null)
        , _port(0) {};
    IpAndPort(const QHostAddress& ip, quint16 port)
        : _ip(ip)
        , _port(port)
    {
    }

    IpAndPort(const QString& ipAndPort)
    {
        const auto parts = ipAndPort.split(':');
        if (parts.size() != 2)
            throw std::runtime_error("invalid address format");

        auto ok = false;
        const auto& port = parts.at(1).toInt(&ok);
        if (!ok || port <= 0 || port > std::numeric_limits<quint16>::max())
            throw std::runtime_error("invalid port format");

        const auto& ip = parts.at(0);
        if (ip == "localhost")
        {
            _ip = QHostAddress::LocalHost;
            _port = port;
        }
        else
        {
            _ip = ip;
            _port = port;

            if (_ip.isNull())
                throw std::runtime_error("invalid ip format");
        }
    }

    IpAndPort(const IpAndPort& other)
        : _ip(other.ip())
        , _port(other.port())
    {
    }

    IpAndPort(IpAndPort&& other) noexcept
        : _ip(std::move(other._ip))
        , _port(std::move(other._port))
    {
    }

public:
    IpAndPort& operator=(const IpAndPort& other)
    {
        if (this != &other)
        {
            _ip = other._ip;
            _port = other._port;
        }
        return *this;
    }

    IpAndPort& operator=(IpAndPort&& other)
    {
        if (this != &other)
        {
            _ip = std::move(other._ip);
            _port = std::move(other._port);
        }
        return *this;
    }

public:
    inline QVariant toVariant() const
    {
        return QVariant::fromValue(toString());
    }

    static IpAndPort fromVariant(const QVariant& variant)
    {
        return IpAndPort::fromString(variant.toString());
    }

    inline QJsonValue toJson() const
    {
        return QJsonValue(toString());
    }

    static IpAndPort fromJson(const QJsonValue& str)
    {
        return IpAndPort(str.toString());
    }

    inline QString toString() const
    {
        static const auto temp = QStringLiteral("%1:%2");
        return temp.arg(_ip.toString(), QString::number(_port));
    }

    static IpAndPort fromString(const QString& str)
    {
        return IpAndPort(str);
    }

public:
    inline const QHostAddress& ip() const&
    {
        return _ip;
    }

    inline QHostAddress ip() const&&
    {
        return _ip;
    }

    inline quint16 port() const
    {
        return _port;
    }

    inline bool isValid() const
    {
        return _port != 0 && !_ip.isNull();
    }

public:
    inline bool operator==(const IpAndPort& other) const
    {
        return _ip == other._ip && _port == other._port;
    }

    inline bool operator!=(const IpAndPort& other) const
    {
        return !this->operator==(other);
    }

    inline operator QString() const
    {
        return toString();
    }

public:
    friend QDebug operator<<(QDebug debug, const IpAndPort& v)
    {
        debug.nospace() << v.toString();
        return debug.maybeSpace();
    }

private:
    QHostAddress _ip;
    quint16 _port;

private:
    struct MetatypeRegister
    {
        MetatypeRegister();
    } static __register;
};

inline IpAndPort stringToAddress(const QString& str)
{
    return IpAndPort::fromString(str);
}

inline QString addressToString(const IpAndPort& ipAndPort)
{
    return ipAndPort.toString();
}

} // namespace Toolkit

Q_DECLARE_METATYPE(Toolkit::IpAndPort)

#endif // IPANDPORT_H
