#pragma once

#include "toolkit/ipandport.h"
#include "toolkit_global.h"

namespace Toolkit {

class TOOLKIT_EXPORT JsonExtractor
{
public:
    /**
     * @brief Проверка и парсинг json значения
     * @param obj json объект, содержащий структуру
     * @param key ключ значения
     * @return QJsonValue
     */
    static QJsonValue extractJsonValue(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг json объекта
     * @param obj json объект, содержащий структуру
     * @param key ключ объекта
     * @return QJsonObject
     */
    static QJsonObject extractJsonObject(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг json массива
     * @param obj json объект, содержащий структуру
     * @param key ключ массива
     * @return QJsonArray
     */
    static QJsonArray extractJsonArray(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг строки из json
     * @param obj json объект, содержащий строку
     * @param key ключ строки
     * @return QString
     */
    static QString extractString(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг массива строк из json
     * @param obj json объект, содержащий строки
     * @param key ключ массива строк
     * @return QString
     */
    static QVector<QString> extractStringArray(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг адреса из json
     * @param obj json объект, содержащий адрес
     * @param key ключ адреса
     * @return QHostAddress
     */
    static QHostAddress extractHost(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг массива адресов из json
     * @param obj json объект, содержащий адреса
     * @param key ключ массива адресов
     * @return QVector<QHostAddress>
     */
    static QVector<QHostAddress> extractHostArray(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг адреса и порта из json
     * @param obj json объект, содержащий строку адреса и порта
     * @param key ключ адреса и порта
     * @return QHostAddress
     */
    static IpAndPort extractHostPort(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг идентификатора из json
     * @param obj json объект, содержащий идентификатор
     * @param key ключ идентификатора
     * @return QUuid
     */
    static QUuid extractUuid(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг флага из json
     * @param obj json объект, содержащий флаг
     * @param key ключ флага
     * @return bool
     */
    static bool extractBool(const QJsonObject &obj, const QString &key);
    /**
     * @brief Проверка и парсинг числа из json
     * @param obj json объект, содержащий число
     * @param key ключ числа
     * @return T
     */
    template<typename T>
    static T extractNumber(const QJsonObject &obj, const QString &key)
    {
        const auto value = extractJsonValue(obj, key);
        if (!value.isDouble())
            throw std::runtime_error(
                unexpected_value_type_str.arg(key, jsonTypeToString(value)).toStdString());

        const auto check = value.toDouble();
        if (check < std::numeric_limits<T>::lowest() || check > std::numeric_limits<T>::max())
            throw std::runtime_error(
                unexpected_value_range_str.arg(key, QString::number(check)).toStdString());
        return static_cast<T>(check);
    }

private:
    JsonExtractor() = delete;
    JsonExtractor(const JsonExtractor &) = delete;
    JsonExtractor &operator=(const JsonExtractor &) = delete;

private:
    static QString jsonTypeToString(const QJsonValue &value);

private:
    static const QString missing_field_str;
    static const QString unexpected_value_type_str;
    static const QString unexpected_value_range_str;
    static const QString unexpected_value_format_str;
};

} // namespace Toolkit
