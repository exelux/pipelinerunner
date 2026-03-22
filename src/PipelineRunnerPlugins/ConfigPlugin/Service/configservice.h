#pragma once

#include "Common/Services/iconfigservice.h"

#include <shared_mutex>

namespace Config {

class ConfigService : public Common::IConfigService
{
    Q_OBJECT

public:
    explicit ConfigService(QObject *parent = nullptr);

    bool start();
    Common::Config config() const final;

private:
    mutable std::shared_timed_mutex _sync;
    Common::Config _config;
};

} // namespace Config
