#include "pipelinerunner.h"

#include <QDir>
#include <Plt2/Core/PluginService>
#include <Plt2/Core/ServiceLocator>

Q_LOGGING_CATEGORY(pipelinerunner, "pipelinerunner");

// clazy:excludeall=non-pod-global-static
static const auto plugins_path = QStringLiteral("plugins");

PipelineRunner::PipelineRunner(const QString &serviceName, QObject *parent)
    : Plt2::Core::ServiceApplication<QCoreApplication>(serviceName, parent)
{
}

void PipelineRunner::start()
{
    try
    {
        initPluginsService();
    }
    catch (const std::exception &e)
    {
        qCCritical(pipelinerunner) << "failed to load application:" << e.what();
        QCoreApplication::exit(-1);
    }
}

void PipelineRunner::initPluginsService() const
{
    const auto pluginService = QSharedPointer<Plt2::Core::PluginService>::create();
    if (!Plt2::Core::ServiceLocator::addService(pluginService))
        throw std::runtime_error("unable to add plugin service");

    const auto pluginsDirPath = QCoreApplication::applicationDirPath() + QDir::separator()
                                + plugins_path;
    pluginService->setPluginDirectories({pluginsDirPath});

    const auto loadErrors = pluginService->loadPlugins();
    if (!loadErrors.has_value())
        return;

    using Reason = Plt2::Core::PluginService::LoadError::Reason;
    const auto convertReason = [](Reason reason) {
        switch (reason)
        {
        case Reason::NotFound:
            return QStringLiteral("could not find loaded plugin file");
        case Reason::LoadFailure:
            return QStringLiteral("failed to load plugin library");
        case Reason::NotAPlugin:
            return QStringLiteral("the library is not a plugin");
        case Reason::BadCast:
            return QStringLiteral("error plugin interface casting");
        case Reason::Rejected:
            return QStringLiteral("the plugin refused to load");
        case Reason::Duplicate:
            return QStringLiteral("plugin with this version and name has already been loaded");
        case Reason::ConstraintUnsatisfied:
            return QStringLiteral("plugin restrictions not satisfied");
        default:
            return QStringLiteral("unknown error");
        }
    };

    for (const auto &error : qAsConst(loadErrors.value()))
    {
        qCCritical(pipelinerunner) << "failed to load plugin" << error.path << ":"
                                   << convertReason(error.reason) << error.error;
    }
}
