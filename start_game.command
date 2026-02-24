#!/usr/bin/env bash
cd "$(dirname "$0")"
set -e
cmake -S . -B build
cmake --build build -j
./build/openfacto
