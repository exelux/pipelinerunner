#include "Common/Structs/jobinfo.h"
#include "toolkit/jsonextractor.h"

namespace Common {

// clazy:excludeall=non-pod-global-static
static const auto message_id_key = QStringLiteral("message_id");

MessageInfo::MessageInfo(const QJsonObject &obj, uint64_t tag, uint8_t priority)
    : tag(tag)
    , priority(priority)
    , json(obj)
{
    try
    {
        id = Toolkit::JsonExtractor::extractUuid(obj, message_id_key);
    }
    catch (const std::exception &e)
    {
        return;
    }
}

} // namespace Common
