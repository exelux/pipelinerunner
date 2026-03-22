#include "configplugin.h"
#include "Common/Services/loadorder.h"
#include "Service/configservice.h"

#include <QLoggingCategory>
#include <Plt2/Core/ServiceLocator>

Q_LOGGING_CATEGORY(configplugin, "configplugin")

namespace Config {

ConfigPlugin::ConfigPlugin(QObject *parent)
    : Plt2::Core::IPlugin(parent)
{
    qCInfo(configplugin) << "loaded";
}

Plt2::Core::PluginInfo ConfigPlugin::info() const
{
    Plt2::Core::PluginInfo info{};

    info.name = tr("config");
    info.description = tr("config plugin");
    info.order = static_cast<Plt2::Core::PluginOrderType>(Common::Plugin::LoadOrder::Config);
    info.version = QVersionNumber{0, 1};

    return info;
}

void ConfigPlugin::start()
{
    auto service = QSharedPointer<ConfigService>::create();

    if (!Plt2::Core::ServiceLocator::addService(service))
        throw std::runtime_error("failed to add config service");

    if (!service->start())
        throw std::runtime_error("failed to start config service");
}

void ConfigPlugin::stop()
{
    if (auto service = Plt2::Core::ServiceLocator::service<ConfigService>())
        Plt2::Core::ServiceLocator::removeService<ConfigService>();
}

} // namespace Config
