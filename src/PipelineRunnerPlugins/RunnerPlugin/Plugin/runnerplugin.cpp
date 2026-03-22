#include "runnerplugin.h"
#include "Common/Services/loadorder.h"
#include "Service/runnerservice.h"

#include <QLoggingCategory>
#include <Plt2/Core/ServiceLocator>

Q_LOGGING_CATEGORY(runnerplugin, "runnerplugin")

namespace Runner {

RunnerPlugin::RunnerPlugin(QObject *parent)
    : Plt2::Core::IPlugin(parent)
{
    qCInfo(runnerplugin) << "loaded";
}

Plt2::Core::PluginInfo RunnerPlugin::info() const
{
    Plt2::Core::PluginInfo info{};

    info.name = tr("runner");
    info.description = tr("runner plugin");
    info.order = static_cast<Plt2::Core::PluginOrderType>(Common::Plugin::LoadOrder::Runner);
    info.version = QVersionNumber{0, 1};

    return info;
}

void RunnerPlugin::start()
{
    try
    {
        auto service = QSharedPointer<RunnerService>::create();

        if (!Plt2::Core::ServiceLocator::addService(service))
        {
            qCCritical(runnerplugin) << "error add runner service";
            return;
        }

        service->start();
    }
    catch (const std::exception &e)
    {
        qCCritical(runnerplugin) << "failed to start runner service: " << e.what();
    }
}

void RunnerPlugin::stop()
{
    if (auto service = Plt2::Core::ServiceLocator::service<RunnerService>())
        Plt2::Core::ServiceLocator::removeService<RunnerService>();
}

} // namespace Runner
