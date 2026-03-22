# Repository Migration Sequence

This runbook applies the architecture cleanup in safe, incremental commits.

## Scope

- Keep app behavior unchanged.
- Keep repository buildable after each commit.
- Separate source code, external dependencies, and generated outputs.

## Preflight

1. Ensure working tree is clean:

   git status --short

2. Ensure submodules are initialized:

   git submodule update --init --recursive

3. Create a migration branch:

   git checkout -b chore/repo-structure-migration

4. Optional local safety tag:

   git tag pre-structure-migration

## Commit 01 - Standardize generated output ignore rules

Goal: introduce out/ as canonical generated-output root.

Edit [.gitignore](.gitignore):

- Add /out/
- Keep legacy ignores for /build/, /build-debug/, /build-release/, /site/

Commands:

   git add .gitignore
   git commit -m "chore(repo): standardize generated output under out"

Verification:

   git check-ignore -v out/test.txt

## Commit 02 - Update build paths in documentation

Goal: document new canonical paths before changing automation.

Edit:

- [README.md](README.md)
- [docs/build-and-run.md](docs/build-and-run.md)

Replace examples:

- build-debug -> out/build/debug
- build-release -> out/build/release

Commands:

   git add README.md docs/build-and-run.md
   git commit -m "docs(build): adopt out/build/debug and out/build/release conventions"

Verification:

   git grep -n "build-debug\\|build-release" README.md docs/build-and-run.md

## Commit 03 - Switch release script default output path

Goal: align local release automation with new output layout.

Edit [scripts/release.sh](scripts/release.sh):

- Default BUILD_DIR from $ROOT_DIR/build-release to $ROOT_DIR/out/build/release

Commands:

   git add scripts/release.sh
   git commit -m "chore(release): use out/build/release as default build dir"

Verification:

   bash scripts/release.sh

If frontend build is not available locally, verify script wiring only:

   bash -n scripts/release.sh

## Commit 04 - Align CI with new output path

Goal: keep CI and local conventions in sync.

Edit [.github/workflows/ci.yml](.github/workflows/ci.yml):

- Configure with -B out/build/release
- Build from out/build/release
- Upload artifact from out/build/release

Commands:

   git add .github/workflows/ci.yml
   git commit -m "ci: switch release build path to out/build/release"

Verification:

- Push branch and confirm CI matrix passes.

## Commit 05 - Move MkDocs generated site under out/docs/site

Goal: remove generated docs output from repository root.

Edit:

- [mkdocs.yml](mkdocs.yml): set site_dir: out/docs/site
- [.github/workflows/pages.yml](.github/workflows/pages.yml): archive from ./out/docs/site
- [README.md](README.md) if it references root site/
- [docs/docs-workflow.md](docs/docs-workflow.md) if it references root site/

Commands:

   git add mkdocs.yml .github/workflows/pages.yml README.md docs/docs-workflow.md
   git commit -m "docs(mkdocs): move generated site to out/docs/site"

Verification:

   mkdocs build --strict
   test -f out/docs/site/index.html && echo "MkDocs output OK"

## Commit 06 - Deprecation note for legacy output folders

Goal: avoid team confusion during transition.

Edit [README.md](README.md):

- Add short note: build-debug, build-release, site are legacy paths.

Commands:

   git add README.md
   git commit -m "docs(repo): document deprecation of legacy output folders"

Verification:

   git grep -n "legacy\|deprecated" README.md

## Commit 07 - Move JUCE submodule under vendor/JUCE

Goal: isolate third-party dependencies from project source.

Preparation:

   mkdir -p vendor

Move and update metadata:

1) Move submodule path:

   git mv JUCE vendor/JUCE

2) Edit [.gitmodules](.gitmodules):

- path = vendor/JUCE

3) Update references in [CMakeLists.txt](CMakeLists.txt) from JUCE to vendor/JUCE.

4) Sync submodule config:

   git submodule sync --recursive

5) Ensure submodule is active:

   git submodule update --init --recursive

Commands:

   git add .gitmodules CMakeLists.txt vendor/JUCE
   git commit -m "refactor(submodules): move JUCE under vendor/JUCE"

Verification:

   cmake -S . -B out/build/debug -DCMAKE_BUILD_TYPE=Debug
   cmake --build out/build/debug --config Debug

## Commit 08 - Move Web UI submodule under vendor/web-ui

Goal: complete dependency boundary cleanup.

Move and update metadata:

1) Move submodule path:

   git mv web-ui vendor/web-ui

2) Edit [.gitmodules](.gitmodules):

- path = vendor/web-ui

3) Update references:

- [CMakeLists.txt](CMakeLists.txt): ONAIRDECK_WEBUI_SUBMODULE_DIR
- [scripts/release.sh](scripts/release.sh): default FRONTEND_DIR
- [README.md](README.md)
- [docs/dependencies.md](docs/dependencies.md)
- [docs/web-ui.md](docs/web-ui.md)

4) Sync and refresh:

   git submodule sync --recursive
   git submodule update --init --recursive

Commands:

   git add .gitmodules CMakeLists.txt scripts/release.sh README.md docs/dependencies.md docs/web-ui.md vendor/web-ui
   git commit -m "refactor(submodules): move web-ui under vendor/web-ui"

Verification:

   cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release
   cmake --build out/build/release --config Release

## Commit 09 - Cleanup remaining legacy path references

Goal: remove stale paths and ensure consistency.

Commands:

   git grep -n "\\bJUCE/\\|\\bweb-ui/\\|build-debug\\|build-release\\|/site/"

Fix remaining legitimate stale references in:

- [README.md](README.md)
- [docs/build-and-run.md](docs/build-and-run.md)
- [docs/web-ui.md](docs/web-ui.md)
- [docs/dependencies.md](docs/dependencies.md)
- [CMakeLists.txt](CMakeLists.txt)
- [scripts/release.sh](scripts/release.sh)
- [.github/workflows/ci.yml](.github/workflows/ci.yml)
- [.github/workflows/pages.yml](.github/workflows/pages.yml)

Commit:

   git add -A
   git commit -m "chore(paths): remove legacy path references after migration"

Verification:

   git grep -n "build-debug\\|build-release\\|/site/"

## Commit 10 - Optional src modularization

Goal: improve scalability without behavior changes.

Possible target structure under [src](src):

- src/app/
- src/ui/
- src/bridge/
- src/engine/

Rules:

- Move files only.
- Keep behavior identical.
- Keep includes and CMake source discovery working.

Commit example:

   git add -A
   git commit -m "refactor(src): introduce modular source layout without behavior changes"

Verification:

   cmake -S . -B out/build/debug -DCMAKE_BUILD_TYPE=Debug
   cmake --build out/build/debug --config Debug

## Final Validation

1. Debug build works:

   cmake -S . -B out/build/debug -DCMAKE_BUILD_TYPE=Debug
   cmake --build out/build/debug --config Debug

2. Release build works:

   cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release
   cmake --build out/build/release --config Release

3. Docs build works:

   mkdocs build --strict

4. Submodules healthy:

   git submodule status --recursive

## Rollback Strategy

- Roll back one commit at a time if a step fails:

   git revert <commit_sha>

- For submodule path issues:

   git submodule sync --recursive
   git submodule update --init --recursive

- Keep migration branch until CI and local validation are green.
