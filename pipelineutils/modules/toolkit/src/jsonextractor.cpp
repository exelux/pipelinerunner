#include "toolkit/jsonextractor.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

namespace Toolkit {
const QString JsonExtractor::missing_field_str = QStringLiteral("missing field: %1");
const QString JsonExtractor::unexpected_value_type_str = QStringLiteral("unexpected value type: %1 %2");
const QString JsonExtractor::unexpected_value_range_str = QStringLiteral("unexpected value range: %1 %2");
const QString JsonExtractor::unexpected_value_format_str = QStringLiteral("unexpected value format: %1 %2");

// clazy:excludeall=non-pod-global-static
static const auto null_key = QStringLiteral("null");
static const auto bool_key = QStringLiteral("bool");
static const auto array_key = QStringLiteral("array");
static const auto object_key = QStringLiteral("object");
static const auto double_key = QStringLiteral("double");
static const auto string_key = QStringLiteral("string");
static const auto undefined_key = QStringLiteral("undefined");

QJsonValue Toolkit::JsonExtractor::extractJsonValue(const QJsonObject &obj, const QString &key)
{
    const auto valueIt = obj.constFind(key);
    if (valueIt == obj.constEnd())
        throw std::runtime_error(missing_field_str.arg(key).toStdString());
    return *valueIt;
}

QJsonObject Toolkit::JsonExtractor::extractJsonObject(const QJsonObject &obj, const QString &key)
{
    const auto value = extractJsonValue(obj, key);
    if (!value.isObject())
        throw std::runtime_error(
            unexpected_value_type_str.arg(key, jsonTypeToString(value)).toStdString());
    return value.toObject();
}

QJsonArray Toolkit::JsonExtractor::extractJsonArray(const QJsonObject &obj, const QString &key)
{
    const auto value = extractJsonValue(obj, key);
    if (!value.isArray())
        throw std::runtime_error(
            unexpected_value_type_str.arg(key, jsonTypeToString(value)).toStdString());
    return value.toArray();
}

QString Toolkit::JsonExtractor::extractString(const QJsonObject &obj, const QString &key)
{
    const auto value = extractJsonValue(obj, key);
    if (!value.isString())
        throw std::runtime_error(
            unexpected_value_type_str.arg(key, jsonTypeToString(value)).toStdString());
    return value.toString();
}

QVector<QString> JsonExtractor::extractStringArray(const QJsonObject &obj, const QString &key)
{
    const auto array = extractJsonArray(obj, key);

    QVector<QString> result(array.size());
    std::transform(array.constBegin(), array.constEnd(), result.begin(), [&key](const auto &val) {
        if (!val.isString())
            throw std::runtime_error(
                unexpected_value_type_str.arg(key, jsonTypeToString(val)).toStdString());
        return val.toString();
    });
    return result;
}

QHostAddress Toolkit::JsonExtractor::extractHost(const QJsonObject &obj, const QString &key)
{
    const auto host = QHostAddress(extractString(obj, key));
    if (host.isNull())
        throw std::runtime_error(unexpected_value_format_str.arg(key, null_key).toStdString());
    return host;
}

QVector<QHostAddress> JsonExtractor::extractHostArray(const QJsonObject &obj, const QString &key)
{
    const auto array = extractJsonArray(obj, key);

    QVector<QHostAddress> result(array.size());
    std::transform(array.constBegin(), array.constEnd(), result.begin(), [&key](const QJsonValue &val) {
        if (!val.isString())
            throw std::runtime_error(
                unexpected_value_type_str.arg(key, jsonTypeToString(val)).toStdString());

        const auto host = QHostAddress(val.toString());
        if (host.isNull())
            throw std::runtime_error(unexpected_value_format_str.arg(key, null_key).toStdString());

        return host;
    });
    return result;
}

IpAndPort JsonExtractor::extractHostPort(const QJsonObject &obj, const QString &key)
{
    return IpAndPort::fromString(extractString(obj, key));
}

QUuid Toolkit::JsonExtractor::extractUuid(const QJsonObject &obj, const QString &key)
{
    const auto uuid = QUuid(extractString(obj, key));
    if (uuid.isNull())
        throw std::runtime_error(unexpected_value_format_str.arg(key, null_key).toStdString());
    return uuid;
}

bool Toolkit::JsonExtractor::extractBool(const QJsonObject &obj, const QString &key)
{
    const auto value = extractJsonValue(obj, key);
    if (!value.isBool())
        throw std::runtime_error(
            unexpected_value_type_str.arg(key, jsonTypeToString(value)).toStdString());
    return value.toBool();
}

QString JsonExtractor::jsonTypeToString(const QJsonValue &value)
{
    switch (value.type())
    {
    case QJsonValue::Null:
        return null_key;
    case QJsonValue::Bool:
        return bool_key;
    case QJsonValue::Double:
        return double_key;
    case QJsonValue::String:
        return string_key;
    case QJsonValue::Array:
        return array_key;
    case QJsonValue::Object:
        return object_key;
    case QJsonValue::Undefined:
        return undefined_key;
    default:
        return undefined_key;
    }
}

} // namespace Toolkit
