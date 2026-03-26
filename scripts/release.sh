#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
if [[ -n "${FRONTEND_DIR:-}" ]]; then
	FRONTEND_DIR="$FRONTEND_DIR"
elif [[ -d "$ROOT_DIR/vendor/web-ui" ]]; then
	FRONTEND_DIR="$ROOT_DIR/vendor/web-ui"
else
	FRONTEND_DIR="$ROOT_DIR/../on-air-deck-figma"
fi
DEFAULT_BUILD_DIR="$ROOT_DIR/out/build/release"
BUILD_DIR="${BUILD_DIR:-$DEFAULT_BUILD_DIR}"
RELEASE_PRESET="${RELEASE_PRESET:-release}"
DIST_DIR="${DIST_DIR:-$FRONTEND_DIR/dist}"

if ! command -v node >/dev/null 2>&1; then
	echo "Error: Node.js is required but was not found in PATH." >&2
	exit 1
fi

if ! command -v npm >/dev/null 2>&1; then
	echo "Error: npm is required but was not found in PATH." >&2
	exit 1
fi

NODE_VERSION_RAW="$(node -v)"
NODE_VERSION_CLEAN="${NODE_VERSION_RAW#v}"
NODE_MAJOR="${NODE_VERSION_CLEAN%%.*}"

if [[ -z "$NODE_MAJOR" || ! "$NODE_MAJOR" =~ ^[0-9]+$ ]]; then
	echo "Error: Unable to parse Node.js version: $NODE_VERSION_RAW" >&2
	exit 1
fi

if (( NODE_MAJOR < 20 )); then
	echo "Error: Node.js >= 20 is required (detected: $NODE_VERSION_RAW)." >&2
	exit 1
fi

echo "Using Node.js $NODE_VERSION_RAW"
echo "Using npm $(npm -v)"

echo "[1/4] Preparing frontend in $FRONTEND_DIR"
cd "$FRONTEND_DIR"

if [[ ! -d node_modules ]]; then
	echo "node_modules not found, installing frontend dependencies"
	if [[ -f package-lock.json ]]; then
		npm ci
	else
		npm install
	fi
fi

echo "[2/4] Building frontend"
npm run build

echo "[3/4] Configuring CMake Release"
cd "$ROOT_DIR"
if [[ "$BUILD_DIR" == "$DEFAULT_BUILD_DIR" ]]; then
	echo "Using CMake preset: $RELEASE_PRESET"
	cmake --preset "$RELEASE_PRESET" -DWEBUI_DIST_PATH="$DIST_DIR"
else
	echo "Using custom BUILD_DIR: $BUILD_DIR"
	cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DWEBUI_DIST_PATH="$DIST_DIR"
fi

echo "[4/4] Building app"
if [[ "$BUILD_DIR" == "$DEFAULT_BUILD_DIR" ]]; then
	cmake --build --preset "$RELEASE_PRESET"
else
	cmake --build "$BUILD_DIR" --config Release
fi

APP_PATH="$BUILD_DIR/OnAirDeck_artefacts/Release/OnAirDeck.app"

echo "Release build completed"
echo "App bundle: $APP_PATH"