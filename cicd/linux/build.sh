#!/bin/bash

usage() {
    echo "Usage: $0 -d project_dir [options]"
    echo "Mandatory arguments:"
    echo "  -d  Project directory path"
    echo "Options:"
    echo "  -q  Qt library path"
    echo "  -c  CMake path"
    echo "  -p  Pipeline id"
    echo "  -h  Show this message and exit"
    echo "Environment variables can be used instead of arguments (used by default values):"
    echo "  PROJECT_DIR, QT_PATH, CMAKE_PATH, PIPELINE_ID"
    echo "Current values of environment variables"
    echo "  PROJECT_DIR = $PROJECT_DIR"
    echo "  QT_PATH = $QT_PATH"
    echo "  CMAKE_PATH = $CMAKE_PATH"
    echo "  PIPELINE_ID = $PIPELINE_ID"
    exit $1
}

check_error() {
    if [ $1 -ne 0 ]; then
        echo "Build completed with error code $1" >&2
        cd "${project_dir}"
        exit $1
    fi
}

project_dir=${PROJECT_DIR}
qt_path=${QT_PATH}
cmake_path=${CMAKE_PATH}
pipeline_id=${PIPELINE_ID}

while getopts ":d:q:c:p:h:" opt; do
    case "${opt}" in
        d)  project_dir=${OPTARG} ;;
        q)  qt_path=${OPTARG} ;;
        c)  cmake_path=${OPTARG} ;;
        p)  pipeline_id=${OPTARG} ;;
        h)  usage 0 ;;
        *)  usage 1 ;;
    esac
done

shift $((OPTIND-1))

if [ -z "$project_dir" ]; then
    echo "Error: Project directory (-d) is required" >&2
    usage 1
fi

if [ ! -d "$project_dir" ]; then
    echo "Error: Project directory $project_dir does not exist" >&2
    exit 1
fi

cd "${project_dir}" || {
    echo "Error: Failed to enter project directory $project_dir" >&2
    exit 1
}

mkdir -p build && cd build || {
    echo "Error: Failed to create or enter build directory" >&2
    exit 1
}

echo "Build script configured with following variables"
echo "  Project dir: ${project_dir}"
echo "  Qt path: ${qt_path}"
echo "  CMake path: ${cmake_path}"
echo "  Pileline id: ${pipeline_id}"

${cmake_path}cmake --version

export NINJA_STATUS="[%f/%t (%r) %c/%o] %e "
cmake_args=(
    -G Ninja "${project_dir}"
    -DCMAKE_BUILD_TYPE=Release
)

if [ -n "$pipeline_id" ]; then
    cmake_args+=(-DPIPELINE_ID="${pipeline_id}")
fi

if [ -n "$qt_path" ]; then
    cmake_args+=(-DCMAKE_PREFIX_PATH="${qt_path}/../")
fi

${cmake_path}cmake "${cmake_args[@]}"
check_error $?

${cmake_path}cmake --build . --config Release
check_error $?

cd "${project_dir}"
check_error $?
