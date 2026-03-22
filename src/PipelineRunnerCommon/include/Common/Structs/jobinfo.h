#pragma once

#include <QDateTime>
#include <QJsonObject>
#include <QUuid>

#include "PipelineRunnerCommon_global.h"

namespace Common {

struct COMMON_EXPORT MessageInfo
{
    uint64_t tag = 0;
    uint8_t priority = 0;

    QUuid id;
    QJsonObject json;

    MessageInfo(const QJsonObject &obj, uint64_t tag, uint8_t priority);
};

} // namespace Common
