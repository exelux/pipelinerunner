#pragma once

#include "Common/Services/ijobhandle.h"
#include "Common/Services/irunnerservice.h"
#include "listenerhandler.h"

#include <QHash>
#include <QTimer>
#include <memory>
#include <shared_mutex>

namespace STL = Plt2::Tools::STL;

namespace Runner {

class RunnerService : public Common::IRunnerService
{
    Q_OBJECT

public:
    explicit RunnerService(QObject *parent = nullptr);

    void start();

private slots:
    void handleJobMessage(const QByteArray &message, uint8_t priority, uint64_t deliveryTag);
    void handleJobFinished(QSharedPointer<Common::IJobHandle> handle);
    void abortAllJobs();

private:
    void checkJobSenderAndListener();
    void sendResultMessage(const QByteArray &data, uint8_t priority) const;

private:
    mutable std::shared_timed_mutex _sync;

    std::unique_ptr<ListenerHandler> _listenerHandler;
    QHash<QUuid, QSharedPointer<Common::IJobHandle>> _jobs;
    QHash<QUuid, uint64_t> _jobsDeliveryTags;
};

} // namespace Runner
