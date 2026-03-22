#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
if [[ -n "${FRONTEND_DIR:-}" ]]; then
	FRONTEND_DIR="$FRONTEND_DIR"
elif [[ -d "$ROOT_DIR/web-ui" ]]; then
	FRONTEND_DIR="$ROOT_DIR/web-ui"
else
	FRONTEND_DIR="$ROOT_DIR/../on-air-deck-figma"
fi
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-release}"
DIST_DIR="${DIST_DIR:-$FRONTEND_DIR/dist}"

echo "[1/3] Building frontend in $FRONTEND_DIR"
cd "$FRONTEND_DIR"
npm run build

echo "[2/3] Configuring CMake Release in $BUILD_DIR"
cd "$ROOT_DIR"
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DWEBUI_DIST_PATH="$DIST_DIR"

echo "[3/3] Building app"
cmake --build "$BUILD_DIR" --config Release

APP_PATH="$BUILD_DIR/OnAirDeck_artefacts/Release/OnAirDeck.app"

echo "Release build completed"
echo "App bundle: $APP_PATH"