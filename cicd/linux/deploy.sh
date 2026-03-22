#!/bin/bash

usage() {
    echo "Usage: $0 -d project_dir [options]"
    echo "Mandatory arguments:"
    echo "  -d  Project directory path"
    echo "Options:"
    echo "  -c  CMake path"
    echo "  -h  Show this message and exit"
    echo "Environment variables can be used instead of arguments (used by default values):"
    echo "  PROJECT_DIR, CMAKE_PATH"
    echo "Current values of environment variables"
    echo "  PROJECT_DIR = $PROJECT_DIR"
    echo "  CMAKE_PATH = $CMAKE_PATH"
    exit $1
}

check_error() {
    if [ $1 -ne 0 ]; then
        echo "Deploy completed with error code $1" >&2
        cd "${project_dir}"
        exit $1
    fi
}

project_dir=${PROJECT_DIR}
cmake_path=${CMAKE_PATH}

while getopts ":d:c:h:" opt; do
    case "${opt}" in
        d)  project_dir=${OPTARG} ;;
        c)  cmake_path=${OPTARG} ;;
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

cd build || {
    echo "Error: Failed to enter deploy directory" >&2
    exit 1
}

echo "Deploy script configured with following variables"
echo "  Project dir: ${project_dir}"
echo "  CMake path: ${cmake_path}"

${cmake_path}cpack -C Release
check_error $?

cd "${project_dir}"
check_error $?
