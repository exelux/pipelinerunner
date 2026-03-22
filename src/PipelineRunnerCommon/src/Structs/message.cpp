#include "Common/Structs/message.h"
#include "toolkit/jsonextractor.h"

#include <QJsonDocument>

namespace Common {

using Extractor = Toolkit::JsonExtractor;

// clazy:excludeall=non-pod-global-static
static const auto message_id_key = QStringLiteral("message_id");
static const auto input_file_key = QStringLiteral("input_file");

QJsonObject messageToJson(const Message &message)
{
    QJsonObject obj;

    obj.insert(message_id_key, message.messageId.toString(QUuid::WithoutBraces));
    obj.insert(input_file_key, message.inputFile);

    return obj;
}

Message messageFromJson(const QJsonObject &obj)
{
    Message message{};

    message.messageId = Extractor::extractUuid(obj, message_id_key);
    message.inputFile = Extractor::extractString(obj, input_file_key);

    return message;
}

QByteArray messageToByteArray(const Message &message)
{
    return QJsonDocument(messageToJson(message)).toJson(QJsonDocument::Compact);
}

} // namespace Common
