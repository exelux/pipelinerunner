#include "Common/Services/iconfigservice.h"

namespace Common {

IConfigService::IConfigService(QObject *parent)
    : Plt2::Core::IServiceT<IConfigService>(parent)
{
}

} // namespace Common
