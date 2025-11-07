#!/usr/bin/env bash
set -euo pipefail

mkdir -p build
rm -f build/tinytcp || true

c++ -Iheaders $(find source -name '*.cpp') -std=c++17 -O2 -g -o build/tinytcp
exec ./build/tinytcp "$@"

