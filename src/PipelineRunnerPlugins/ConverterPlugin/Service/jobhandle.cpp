#include "jobhandle.h"

namespace Converter {

JobHandle::JobHandle(const Common::Message &message, QObject *parent)
    : Common::IJobHandle(parent)
    , _message(message)
    , _result(STL::nullopt)
{
}

Common::Message JobHandle::message() const
{
    std::shared_lock<std::shared_timed_mutex> lock(_sync);
    return _message;
}

STL::optional<QString> JobHandle::result() const
{
    std::shared_lock<std::shared_timed_mutex> lock(_sync);
    return _result;
}

QDateTime JobHandle::startTime() const
{
    std::shared_lock<std::shared_timed_mutex> lock(_sync);
    return _startTime;
}

QDateTime JobHandle::endTime() const
{
    std::shared_lock<std::shared_timed_mutex> lock(_sync);
    return _endTime;
}

void JobHandle::start()
{
    std::lock_guard<std::shared_timed_mutex> lock(_sync);
    _startTime = QDateTime::currentDateTimeUtc();
}

void JobHandle::finish(const STL::optional<QString> &result)
{
    {
        std::lock_guard<std::shared_timed_mutex> lock(_sync);
        _result = result;
        _endTime = QDateTime::currentDateTimeUtc();
    }

    emit finished();
}

} // namespace Converter
