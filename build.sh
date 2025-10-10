#!/usr/bin/env bash
set -e

mkdir -p bin

function safe_runner() {
    cmd="$1"
    msg="$2"
    echo -e "\033[0;34m[  RUN  ]\033[0m $msg"
    if ! $cmd > /dev/null 2>&1; then
        echo -e "\033[0;31m[ Error ]\033[0m $msg"
        $cmd
        exit 1
    else
        echo -e "\033[0;32m[  OK   ]\033[0m $msg"
    fi
}

# Compile step
safe_runner "c++ -std=c++20 $(find source -name '*.cpp') -Iheaders -I/path/to/tomlplusplus/include -o bin/ttcp" "Compiling C++ project"

# Run step
echo -e "\033[0;34m[  RUN  ]\033[0m Executing binary"
./bin/ttcp "$@"

