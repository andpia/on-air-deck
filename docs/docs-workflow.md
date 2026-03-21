# Docs Workflow

This repository uses MkDocs for documentation authoring and GitHub Pages for publication.

## Local Preview

On macOS, system Python may block global `pip install` because of PEP 668. Use a virtual environment instead.

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements-docs.txt
mkdocs serve
```

Local preview URL:

```text
http://127.0.0.1:8000
```

## Strict Build

To validate links and configuration before pushing:

```bash
source .venv/bin/activate
mkdocs build --strict
```

The generated static site is written to `site/` and is ignored by Git.

## Publication

GitHub Pages is deployed from the workflow in `.github/workflows/pages.yml`.

The workflow:

1. checks out the repository
2. installs Python 3.11
3. installs `requirements-docs.txt`
4. runs `mkdocs build --strict`
5. uploads `site/` as the Pages artifact
6. deploys the generated site

## When Deploy Runs

The Pages workflow runs:

- manually with `workflow_dispatch`
- automatically on pushes to `main` when documentation-related files change

## Editing Guidance

Prefer keeping product documentation in focused Markdown pages under `docs/`.

Good candidates for future pages:

- audio engine architecture
- streaming pipeline
- packaging and release process
- frontend integration contract