cmake_minimum_required(VERSION 3.19)
project(RtrHpoRunnerPlugins LANGUAGES CXX)

# ----------
# Subprojects
add_subdirectory(ConfigPlugin)
add_subdirectory(ConverterPlugin)
add_subdirectory(RabbitPlugin)
add_subdirectory(RunnerPlugin)
