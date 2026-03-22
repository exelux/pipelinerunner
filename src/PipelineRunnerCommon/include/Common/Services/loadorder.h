#pragma once

#include <Plt2/Core/PluginService>

namespace Common {
namespace Plugin {

/**
* @brief Порядок инициализации плагинов
*/
enum class LoadOrder : int
{
    BeginLoadOrder = Plt2::Core::PluginOrder::base, ///< неизвестный плагин

    Config,    ///< плагин конфига
    Converter, ///< плагин конвертера
    Rabbit,    ///< плагин взаимодействия с Rabbit
    Runner,    ///< плагин раннера
};

} // namespace Plugin
} // namespace Common
