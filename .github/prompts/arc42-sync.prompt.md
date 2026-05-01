---
description: "Synchronize arc42 docs and diagrams with recent code changes for a target module or changed-file set."
name: "Arc42 Sync"
agent: "Docs Sync Updater"
argument-hint: "Target module path or changed files, e.g. Src/Gen/Com"
---

Update architecture docs to reflect recent implementation changes.

## Steps

1. Identify impacted docs and diagrams from the supplied scope.
2. Update only affected sections in `docs/arc42/*.md`.
3. Update related `docs/components/*.puml` diagrams if behavior/structure changed.
4. Append/update `docs/arc42/_sync-log.md` with date and touched files.
5. Report residual gaps where code changed but evidence remains incomplete.

If the scope is missing, ask the user which module or files to sync.
