#pragma once

#include <QObject>
#include <Plt2/Tools/STL/optional>

#include "Common/Structs/message.h"
#include "PipelineRunnerCommon_global.h"

namespace Common {

namespace STL = Plt2::Tools::STL;

class COMMON_EXPORT IJobHandle : public QObject
{
    Q_OBJECT

public:
    IJobHandle(QObject *parent = nullptr);

    virtual Message message() const = 0;
    virtual STL::optional<QString> result() const = 0;

    virtual QDateTime startTime() const = 0;
    virtual QDateTime endTime() const = 0;

signals:
    void finished();
};

} // namespace Common
