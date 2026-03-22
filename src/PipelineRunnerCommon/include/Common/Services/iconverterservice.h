#pragma once

#include <Plt2/Core/Interface/IService>

#include "PipelineRunnerCommon_global.h"
#include "ijobhandle.h"

namespace Common {

class COMMON_EXPORT IConverterService : public Plt2::Core::IServiceT<IConverterService>
{
    Q_OBJECT

public:
    explicit IConverterService(QObject *parent = nullptr);

    virtual QSharedPointer<IJobHandle> registerJobMessage(const Message &message) = 0;
    virtual bool processJobMessage(QSharedPointer<IJobHandle> handle) = 0;
};

} // namespace Common
