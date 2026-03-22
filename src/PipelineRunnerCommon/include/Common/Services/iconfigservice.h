#pragma once

#include <Plt2/Core/Interface/IService>

#include "Common/Structs/config.h"
#include "PipelineRunnerCommon_global.h"

namespace Common {

class COMMON_EXPORT IConfigService : public Plt2::Core::IServiceT<IConfigService>
{
    Q_OBJECT

public:
    explicit IConfigService(QObject *parent = nullptr);

    virtual Config config() const = 0;
};

} // namespace Common
