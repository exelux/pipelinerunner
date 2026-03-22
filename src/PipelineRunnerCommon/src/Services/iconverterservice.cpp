#include "Common/Services/iconverterservice.h"

namespace Common {

IConverterService::IConverterService(QObject *parent)
    : Plt2::Core::IServiceT<IConverterService>(parent)
{
}

} // namespace Common
