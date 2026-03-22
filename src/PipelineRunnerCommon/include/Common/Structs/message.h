#pragma once

#include <QJsonObject>
#include <QUuid>

#include "PipelineRunnerCommon_global.h"

namespace Common {

struct COMMON_EXPORT Message
{
    QUuid messageId;
    QString inputFile;
};

QJsonObject messageToJson(const Message& message);
QByteArray messageToByteArray(const Message& message);
Message messageFromJson(const QJsonObject& obj);

} // namespace Common
