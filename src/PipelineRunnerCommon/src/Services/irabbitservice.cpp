#include "Common/Services/irabbitservice.h"

namespace Common {

IRabbitService::IRabbitService(QObject *parent)
    : Plt2::Core::IServiceT<IRabbitService>(parent)
{
}

} // namespace Common
