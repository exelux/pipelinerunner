#pragma once

#include "Common/Services/iconverterservice.h"
#include "Common/Structs/config.h"
#include "jobhandle.h"

#include <QProcess>
#include <queue>
#include <shared_mutex>

namespace Converter {
namespace STL = Plt2::Tools::STL;

struct JobEnv
{
    QString tmpDir;
    QString inputTmpDir;
    QString outputTmpDir;
    QString inputTmpFile;
    QString outputTmpFile;
    void clear();
};

class ConverterService : public Common::IConverterService
{
    Q_OBJECT

public:
    explicit ConverterService(QObject *parent = nullptr);

    QSharedPointer<Common::IJobHandle> registerJobMessage(const Common::Message &message) final;
    bool processJobMessage(QSharedPointer<Common::IJobHandle> handle) final;

private:
    QSharedPointer<JobHandle> currentJob() const;

    void startJob();
    void endJob(const STL::optional<QString> &result);
    bool notifyJob();
    bool createEnv();
    bool startConverter();
    bool createDirIfNotExists(const QString &path);

    STL::optional<QString> copyResultFile();

private slots:
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleProcessError(QProcess::ProcessError error);

private:
    mutable std::shared_timed_mutex _mutex;

    Common::Config _config;

    JobEnv _jobEnv;
    QProcess _process;
    std::queue<QWeakPointer<JobHandle>> _receivedMessages;
};
} // namespace Converter
