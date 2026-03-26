.DEFAULT_GOAL := help

.PHONY: help configure-debug build-debug configure-release build-release release clean-safe clean-full clean-deep

help:
	@echo "OnAirDeck build helpers"
	@echo "  make configure-debug   # cmake --preset debug"
	@echo "  make build-debug       # cmake --build --preset debug"
	@echo "  make configure-release # cmake --preset release"
	@echo "  make build-release     # cmake --build --preset release"
	@echo "  make release           # bash scripts/release.sh"
	@echo "  make clean-safe        # bash scripts/clean.sh safe"
	@echo "  make clean-full        # bash scripts/clean.sh full"
	@echo "  make clean-deep        # bash scripts/clean.sh deep"

configure-debug:
	cmake --preset debug

build-debug: configure-debug
	cmake --build --preset debug

configure-release:
	cmake --preset release

build-release: configure-release
	cmake --build --preset release

release:
	bash scripts/release.sh

clean-safe:
	bash scripts/clean.sh safe

clean-full:
	bash scripts/clean.sh full

clean-deep:
	bash scripts/clean.sh deep
