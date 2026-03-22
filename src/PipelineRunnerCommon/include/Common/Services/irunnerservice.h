#pragma once

#include <Plt2/Core/Interface/IService>

#include "PipelineRunnerCommon_global.h"

namespace Common {

class COMMON_EXPORT IRunnerService : public Plt2::Core::IServiceT<IRunnerService>
{
    Q_OBJECT

public:
    explicit IRunnerService(QObject *parent = nullptr);
};

} // namespace Common
