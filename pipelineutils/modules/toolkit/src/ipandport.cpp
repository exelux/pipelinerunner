#include "toolkit/ipandport.h"

namespace Toolkit {

IpAndPort::MetatypeRegister IpAndPort::__register{};

IpAndPort::MetatypeRegister::MetatypeRegister()
{
    QMetaType::registerConverter<Toolkit::IpAndPort, QString>(Toolkit::addressToString);
    QMetaType::registerConverter<QString, Toolkit::IpAndPort>(Toolkit::stringToAddress);
    qRegisterMetaType<Toolkit::IpAndPort>("Toolkit::IpAndPort");
}

}
