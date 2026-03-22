#include "rabbitplugin.h"
#include "Common/Services/loadorder.h"
#include "Service/rabbitservice.h"

#include <QLoggingCategory>
#include <Plt2/Core/ServiceLocator>

Q_LOGGING_CATEGORY(rabbitplugin, "rabbitplugin")

namespace Rabbit {

RabbitPlugin::RabbitPlugin(QObject *parent)
    : Plt2::Core::IPlugin(parent)
{
    qCInfo(rabbitplugin) << "loaded";
}

Plt2::Core::PluginInfo RabbitPlugin::info() const
{
    Plt2::Core::PluginInfo info{};

    info.name = tr("rabbit");
    info.description = tr("rabbit plugin");
    info.order = static_cast<Plt2::Core::PluginOrderType>(Common::Plugin::LoadOrder::Rabbit);
    info.version = QVersionNumber{0, 1};

    return info;
}

void RabbitPlugin::start()
{
    try
    {
        auto service = QSharedPointer<RabbitService>::create();
        if (!Plt2::Core::ServiceLocator::addService(service))
        {
            qCCritical(rabbitplugin) << "failed to add rabbit service";
            return;
        }

        service->start();
    }
    catch (const std::exception &e)
    {
        qCCritical(rabbitplugin) << "failed to start rabbit service: " << e.what();
    }
}

void RabbitPlugin::stop()
{
    if (auto service = Plt2::Core::ServiceLocator::service<RabbitService>())
        Plt2::Core::ServiceLocator::removeService<RabbitService>();
}

} // namespace Rabbit
