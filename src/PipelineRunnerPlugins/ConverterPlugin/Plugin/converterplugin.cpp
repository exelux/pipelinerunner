#include "converterplugin.h"
#include "Common/Services/loadorder.h"
#include "Service/converterservice.h"

#include <QLoggingCategory>
#include <Plt2/Core/ServiceLocator>

Q_LOGGING_CATEGORY(converterplugin, "converterplugin")

namespace Converter {

ConverterPlugin::ConverterPlugin(QObject *parent)
    : Plt2::Core::IPlugin(parent)
{
    qCInfo(converterplugin) << "loaded";
}

Plt2::Core::PluginInfo ConverterPlugin::info() const
{
    Plt2::Core::PluginInfo info{};

    info.name = tr("converter");
    info.description = tr("converter plugin");
    info.order = static_cast<Plt2::Core::PluginOrderType>(Common::Plugin::LoadOrder::Converter);
    info.version = QVersionNumber{0, 1};

    return info;
}

void ConverterPlugin::start()
{
    auto service = QSharedPointer<ConverterService>::create();

    if (!Plt2::Core::ServiceLocator::addService(service))
    {
        qCCritical(converterplugin) << "error add converter service";
        return;
    }
}

void ConverterPlugin::stop()
{
    if (auto service = Plt2::Core::ServiceLocator::service<ConverterService>())
        Plt2::Core::ServiceLocator::removeService<ConverterService>();
}

} // namespace Converter
