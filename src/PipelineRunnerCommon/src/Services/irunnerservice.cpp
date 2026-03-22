#include "Common/Services/irunnerservice.h"

namespace Common {

IRunnerService::IRunnerService(QObject *parent)
    : Plt2::Core::IServiceT<IRunnerService>(parent)
{
}

} // namespace Common
