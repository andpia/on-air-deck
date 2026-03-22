#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
MODE="${1:-safe}"
CONFIRM_FLAG="${2:-}"

usage() {
	cat <<'EOF'
Usage:
  bash scripts/clean.sh [safe|full|pristine] [--yes]

Modes:
  safe      Remove generated output under out/
  full      Remove out/ plus frontend dist/ and node_modules/
  pristine  Run git clean -fdx in the repo and all submodules

Notes:
  - pristine is destructive and requires --yes
  - run from anywhere; the script resolves the repository root automatically
EOF
}

remove_path() {
	local path="$1"
	if [[ -e "$path" ]]; then
		echo "Removing $path"
		rm -rf "$path"
	else
		echo "Skipping $path (not present)"
	fi
}

run_safe_clean() {
	remove_path "$ROOT_DIR/out"
}

run_full_clean() {
	run_safe_clean
	remove_path "$ROOT_DIR/vendor/web-ui/dist"
	remove_path "$ROOT_DIR/vendor/web-ui/node_modules"
}

run_pristine_clean() {
	if [[ "$CONFIRM_FLAG" != "--yes" ]]; then
		echo "Refusing to run pristine clean without --yes"
		echo "This would delete all untracked and ignored files in the repo and submodules."
		exit 1
	fi

	cd "$ROOT_DIR"
	echo "Running pristine clean in repository root"
	git clean -fdx
	echo "Running pristine clean in submodules"
	git submodule foreach --recursive git clean -fdx
}

case "$MODE" in
	safe)
		run_safe_clean
		;;
	full)
		run_full_clean
		;;
	pristine)
		run_pristine_clean
		;;
	-h|--help|help)
		usage
		;;
	*)
		echo "Unknown mode: $MODE"
		usage
		exit 1
		;;
esac