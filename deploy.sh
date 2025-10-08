#!/usr/bin/env bash
set -e

g++ src/tcp.cpp -o bin/tcp
./bin/tcp "$@"
