#include "configservice.h"
#include "toolkit/jsonextractor.h"

#include <QCoreApplication>
#include <QFile>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <mutex>

Q_DECLARE_LOGGING_CATEGORY(configplugin);

namespace Config {

using Extractor = Toolkit::JsonExtractor;

// clazy:excludeall=non-pod-global-static
static const auto config_path = QStringLiteral("/etc/pipelinerunner/config.json");
static const auto rabbit_host_key = QStringLiteral("rabbit_host");
static const auto rabbit_port_key = QStringLiteral("rabbit_port");
static const auto rabbit_login_key = QStringLiteral("rabbit_login");
static const auto sub_queue_name_key = QStringLiteral("sub_queue_name");
static const auto pub_queue_name_key = QStringLiteral("pub_queue_name");
static const auto rabbit_password_key = QStringLiteral("rabbit_password");
static const auto converter_path_key = QStringLiteral("converter_path");
static const auto data_storage_key = QStringLiteral("data_storage");

inline Common::Config parseConfig(const QJsonObject &obj)
{
    Common::Config config{};

    config.rabbitHost = Extractor::extractHost(obj, rabbit_host_key);
    config.rabbitPort = Extractor::extractNumber<uint16_t>(obj, rabbit_port_key);
    config.rabbitLogin = Extractor::extractString(obj, rabbit_login_key);
    config.rabbitPassword = Extractor::extractString(obj, rabbit_password_key);
    config.subQueueName = Extractor::extractString(obj, sub_queue_name_key);
    config.pubQueueName = Extractor::extractString(obj, pub_queue_name_key);
    config.converterPath = Extractor::extractString(obj, converter_path_key);
    config.dataStorage = Extractor::extractString(obj, data_storage_key);

    return config;
}

ConfigService::ConfigService(QObject *parent)
    : Common::IConfigService(parent)
{
}

bool ConfigService::start()
{
    const auto args = QCoreApplication::arguments();
    QString configPath = config_path;

    if (args.size() == 2)
        configPath = args[1];

    qCInfo(configplugin) << "config path:" << configPath;

    QFile configFile(configPath);
    if (!configFile.open(QIODevice::ReadOnly))
        throw std::runtime_error(QStringLiteral("unable to access config file: %1 (%2)")
                                     .arg(configPath, configFile.errorString())
                                     .toStdString());

    QJsonParseError parseError;
    const auto jsonData = QJsonDocument::fromJson(configFile.readAll(), &parseError);
    configFile.close();

    if (parseError.error != QJsonParseError::NoError)
        throw std::runtime_error(QStringLiteral("json parse error at %1: %2")
                                     .arg(parseError.offset)
                                     .arg(parseError.errorString())
                                     .toStdString());

    try
    {
        auto config = parseConfig(jsonData.object());
        {
            std::lock_guard<std::shared_timed_mutex> lock(_sync);
            _config = std::move(config);
        }
        qCInfo(configplugin) << "config loaded successfully";
    }
    catch (const std::exception &e)
    {
        qCCritical(configplugin) << "failed to load config:" << e.what();
        return false;
    }

    return true;
}

Common::Config ConfigService::config() const
{
    std::shared_lock<std::shared_timed_mutex> lock(_sync);
    return _config;
}

} // namespace Config
