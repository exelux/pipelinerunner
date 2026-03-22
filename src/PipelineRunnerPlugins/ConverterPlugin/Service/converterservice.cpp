#include "converterservice.h"
#include "Common/Services/iconfigservice.h"
#include "jobhandle.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <Plt2/Core/ServiceLocator>

Q_DECLARE_LOGGING_CATEGORY(converterplugin)

namespace Converter {

// clazy:excludeall=non-pod-global-static
static const auto tmp_dir_template = QStringLiteral("/tmp/%1/");
static const auto output_file_template = QStringLiteral("%1/%2-[%3]-[%4].%5");

ConverterService::ConverterService(QObject *parent)
    : Common::IConverterService(parent)
{
    const auto configService = Plt2::Core::ServiceLocator::service<Common::IConfigService>();
    if (!configService)
    {
        qCCritical(converterplugin) << "config service unavailable";
        return;
    }

    _config = configService->config();

    connect(&_process,
            qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this,
            &ConverterService::handleProcessFinished);
    connect(&_process, &QProcess::errorOccurred, this, &ConverterService::handleProcessError);
    connect(&_process, &QProcess::readyReadStandardOutput, this, [this]() {
        qCInfo(converterplugin) << "converter output:" << _process.readAllStandardOutput();
    });
}

QSharedPointer<Common::IJobHandle> ConverterService::registerJobMessage(
    const Common::Message &message)
{
    return QSharedPointer<JobHandle>::create(message);
}

bool ConverterService::processJobMessage(QSharedPointer<Common::IJobHandle> handle)
{
    const auto castedHandle = handle.objectCast<JobHandle>();
    if (!castedHandle)
    {
        qCCritical(converterplugin) << "unable to cast job handle";
        return false;
    };

    const auto needJobStart = [this](const QSharedPointer<JobHandle> &handle) {
        std::lock_guard<std::shared_timed_mutex> lock(_mutex);
        _receivedMessages.push(handle);
        return _receivedMessages.size() == 1;
    };

    if (needJobStart(castedHandle))
        QMetaObject::invokeMethod(this, &ConverterService::startJob, Qt::QueuedConnection);

    return handle;
}

QSharedPointer<JobHandle> ConverterService::currentJob() const
{
    std::shared_lock<std::shared_timed_mutex> lock(_mutex);
    return _receivedMessages.front().toStrongRef();
}

void ConverterService::startJob()
{
    qCInfo(converterplugin) << "job taken";

    if (!notifyJob())
        return endJob(STL::nullopt);

    if (!createEnv())
        return endJob(STL::nullopt);

    if (!startConverter())
        return endJob(STL::nullopt);
}

void ConverterService::endJob(const STL::optional<QString> &result)
{
    if (auto handle = currentJob())
        handle->finish(result);

    _jobEnv.clear();

    const auto needJobStart = [this]() {
        std::lock_guard<std::shared_timed_mutex> lock(_mutex);
        _receivedMessages.pop();
        return !_receivedMessages.empty();
    };

    if (needJobStart())
        startJob();
}

bool ConverterService::notifyJob()
{
    if (const auto job = currentJob())
    {
        job->start();
        return true;
    }
    else
    {
        qCCritical(converterplugin) << "task is no longer needed";
        return false;
    }
}

bool ConverterService::createEnv()
{
    const auto job = currentJob();
    if (!job)
    {
        qCCritical(converterplugin) << "task is no longer needed";
        return false;
    }

    const auto uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    _jobEnv.tmpDir = tmp_dir_template.arg(uuid);
    _jobEnv.inputTmpDir = _jobEnv.tmpDir + "input/";
    _jobEnv.outputTmpDir = _jobEnv.tmpDir + "output/";

    if (!createDirIfNotExists(_jobEnv.inputTmpDir))
        return false;

    if (!createDirIfNotExists(_jobEnv.outputTmpDir))
        return false;

    const auto inputFilePath = QDir::cleanPath(_config.dataStorage + QDir::separator()
                                               + job->message().inputFile);
    const auto inputFileInfo = QFileInfo(inputFilePath);
    const auto converterFileInfo = QFileInfo(_config.converterPath);

    _jobEnv.inputTmpFile = _jobEnv.inputTmpDir + inputFileInfo.fileName();
    _jobEnv.outputTmpFile = QDir::cleanPath(
        output_file_template.arg(_jobEnv.outputTmpDir,
                                 inputFileInfo.baseName(),
                                 converterFileInfo.baseName(),
                                 QDateTime::currentDateTime().toString("hh-mm-ss-zzz"),
                                 inputFileInfo.suffix()));

    if (!QFile::copy(inputFilePath, _jobEnv.inputTmpFile))
    {
        qCCritical(converterplugin) << "unable to copy input file";
        return false;
    }

    return true;
}

bool ConverterService::startConverter()
{
    const auto job = currentJob();
    if (!job)
    {
        qCCritical(converterplugin) << "task is no longer needed";
        return false;
    }

    const auto message = job->message();

    QStringList args;
    args << _config.converterPath;
    args << _jobEnv.inputTmpFile;
    args << _jobEnv.outputTmpFile;

    _process.start("python3", args);

    return true;
}

bool ConverterService::createDirIfNotExists(const QString &path)
{
    QDir dir(path);

    if (!dir.exists())
        if (!dir.mkpath(path))
        {
            qCCritical(converterplugin) << "unable to create dir:" + path;
            return false;
        }

    return true;
}

STL::optional<QString> ConverterService::copyResultFile()
{
    const auto job = currentJob();
    if (!job)
    {
        qCCritical(converterplugin) << "task is no longer needed";
        return STL::nullopt;
    }

    const auto outputFileInfo = QFileInfo(_jobEnv.outputTmpFile);
    const auto outputFile = QDir::cleanPath(_config.dataStorage + QDir::separator()
                                            + outputFileInfo.fileName());

    if (!QFile::copy(_jobEnv.outputTmpFile, outputFile))
    {
        qCCritical(converterplugin) << "unable to copy output file";
        return STL::nullopt;
    }

    return outputFileInfo.fileName();
}

void ConverterService::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0 && exitStatus == QProcess::NormalExit)
    {
        endJob(copyResultFile());
    }
    else
    {
        qCCritical(converterplugin)
            << _process.readAllStandardOutput() << _process.readAllStandardError() << exitCode;
        endJob(STL::nullopt);
    }
}

void ConverterService::handleProcessError(QProcess::ProcessError error)
{
    qCCritical(converterplugin) << _process.errorString();
    endJob(STL::nullopt);
}

void JobEnv::clear()
{
    if (tmpDir.isEmpty())
        return;

    QDir(tmpDir).removeRecursively();
    tmpDir.clear();
    inputTmpDir.clear();
    outputTmpDir.clear();
    inputTmpFile.clear();
}

} // namespace Converter
