#pragma once

#include "Common/Services/ijobhandle.h"

#include <QDateTime>
#include <shared_mutex>

namespace Converter {
namespace STL = Plt2::Tools::STL;

class JobHandle : public Common::IJobHandle
{
    Q_OBJECT

public:
    JobHandle(const Common::Message &message, QObject *parent = nullptr);

    Common::Message message() const final;
    STL::optional<QString> result() const final;

    QDateTime startTime() const final;
    QDateTime endTime() const final;

public:
    void start();
    void finish(const STL::optional<QString> &result);

private:
    mutable std::shared_timed_mutex _sync;

    QDateTime _startTime;
    QDateTime _endTime;

    Common::Message _message;
    STL::optional<QString> _result;
};

} // namespace Converter
