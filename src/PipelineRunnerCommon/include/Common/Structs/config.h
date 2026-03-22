#pragma once

#include <QHostAddress>

#include "PipelineRunnerCommon_global.h"

namespace Common {

struct COMMON_EXPORT Config
{
    QHostAddress rabbitHost;
    int rabbitPort = 0;
    QString rabbitLogin;
    QString rabbitPassword;
    QString subQueueName;
    QString pubQueueName;
    QString converterPath;
    QString dataStorage;
};

} // namespace Common
