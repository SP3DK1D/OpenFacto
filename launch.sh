#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_TYPE="${1:-Debug}"
BUILD_DIR="$SCRIPT_DIR/build"

if ! command -v cmake >/dev/null 2>&1; then
  echo "Error: cmake not found in PATH" >&2
  exit 1
fi

GENERATOR=""
if command -v ninja >/dev/null 2>&1; then
  GENERATOR="-G Ninja"
fi

mkdir -p "$BUILD_DIR"
if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
  cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" $GENERATOR -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
else
  cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
fi

cmake --build "$BUILD_DIR" --config "$BUILD_TYPE"

EXE="$BUILD_DIR/bin/openfacto"
if [ -x "$EXE" ]; then
  cd "$SCRIPT_DIR"
  "$EXE"
elif [ -x "$BUILD_DIR/openfacto" ]; then
  cd "$SCRIPT_DIR"
  "$BUILD_DIR/openfacto"
else
  echo "Error: built executable not found." >&2
  exit 1
fi
