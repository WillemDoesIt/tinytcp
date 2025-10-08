#!/usr/bin/env bash
set -e

g++ src/main.cpp -o bin/ttcp
./bin/ttcp "$@"
