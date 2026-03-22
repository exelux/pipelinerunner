#pragma once

#include <Plt2/Core/Interface/IPlugin>

namespace Rabbit {

class RabbitPlugin : public Plt2::Core::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(Plt2::Core::IPlugin)
    Q_PLUGIN_METADATA(IID "Plt2::Core::IPlugin")

public:
    explicit RabbitPlugin(QObject *parent = nullptr);

    // IPlugin interface
public:
    Plt2::Core::PluginInfo info() const final;

    void start() final;
    void stop() final;
};

} // namespace Rabbit
