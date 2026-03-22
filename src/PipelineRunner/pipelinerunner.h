#pragma once

#include <QCoreApplication>
#include <Plt2/Core/ServiceApplication>

class PipelineRunner : public Plt2::Core::ServiceApplication<QCoreApplication>
{
public:
    PipelineRunner(const QString &serviceName, QObject *parent = nullptr);
    void start() override final;

private:
    void initPluginsService() const;
};
