# AI Agents and Prompts

This repository includes reusable Copilot agent/prompt definitions for 

- architecture documentation
- MISRA / static analysis
- Coding, testing, and refactoring workflows

Where to find:

- Agent definitions: `.github/agents/`
- Prompt definitions: `.github/prompts/`

## Architecture documentation

The arc42 + PlantUML prompt set added for documentation workflows is:

- `arc42-create-architect`
- `arc42-extract-context`
- `arc42-create-update-diagram`
- `arc42-write`
- `arc42-review`
- `arc42-sync`

### How To Use

Run these prompts in Copilot Chat using slash commands (prompt filename without `.prompt.md`).

Examples:

```text
/arc42-create-architect Src/Prj/App/Signal
/arc42-extract-context Src/Prj/App/Signal
/arc42-create-update-diagram Src/Prj/App/Signal runtime
/arc42-write Src/Prj/App/Signal
/arc42-review docs/arc42/signal.md
/arc42-sync Src/Prj/App/Signal
```

### Recommended Flow

1. Extract context from code.
2. Build/refresh arc42 structure.
3. Generate/update PlantUML views.
4. Write full arc42 narrative.
5. Review for consistency and evidence.
6. Sync docs after future code changes.

Typical sequence:

```text
/arc42-context <module>
/arc42-architect <module>
/arc42-diagram <module> runtime
/arc42-write <module>
/arc42-review docs/arc42/<module>.md
```

### Prompt Quick Reference

#### /arc42-create-architect

Creates or refines arc42 section structure for a module.

Input:

- Module path (example: `Src/Prj/App/Signal`)

Output:

- `docs/arc42/<module>.md` scaffold and filled sections where evidence exists.

#### /arc42-extract-context

Extracts implementation-grounded architecture facts from source/build files.

Input:

- Module path (folder or file scope)

Output:

- `docs/arc42/_context/<module>-context.md`

#### /arc42-create-update-diagram

Creates/updates PlantUML diagrams aligned to arc42 views.

Input:

- Module path and optional view (`building-block`, `runtime`, `deployment`, `class`)

Output:

- `.puml` files in `docs/components/`

#### /arc42-write

Writes or updates complete arc42 Markdown using extracted evidence.

Input:

- Module path

Output:

- `docs/arc42/<module>.md` with concrete content and known gaps.

#### /arc42-review

Performs QA on arc42 docs/diagrams and reports issues by severity.

Input:

- Documentation file or module scope

Output:

- Structured findings and fix suggestions.

#### /arc42-sync

Updates only impacted docs/diagrams after code changes.

Input:

- Changed module/files

Output:

- Targeted updates in `docs/arc42/`, `docs/components/`, and sync log updates.

### Tips

- Keep inputs explicit (module path + optional viewpoint) for best results.
- Treat generated docs as draft architecture assets and review them in PRs.
- Re-run `/arc42-sync` after major refactors to prevent documentation drift.

## MISRA and static analysis

These are available as slash commands from `.github/prompts/`:

- `/misra-review`
- `/misra-fix`

Examples:

```text
/misra-review Src/Gen/Dcc/BitExtractor.h
/misra-review Src/Prj/App/Signal
/misra-fix Src/Prj/App/Signal/DccDecoder.cpp: [Line 113] implicit narrowing conversion
```

### /misra-review

Purpose:

- Run MISRA compliance analysis for a file or folder.

Typical output:

- Structured findings by severity (CRITICAL/HIGH/MEDIUM/COMPLIANT).
- Report files under `.github/analysis/misra/...`.

Use when:

- You want a static-analysis style audit before refactoring or feature work.

### /misra-fix

Purpose:

- Apply exactly one MISRA violation fix, validate build, and update analysis status.

Use when:

- You already have a specific violation to fix (often from `/misra-review`).

Note:

- This flow is intentionally one-fix-per-run.

### MISRA Reviewer

Use for:

- Analysis-only MISRA audits with structured reporting.

Good input examples:

- "Review this folder for MISRA 2008/2023 violations and produce severity-ranked findings."

### Suggested Combined Workflows

```text
/misra-review <file-or-module>
/misra-fix <single violation>
/misra-review <same scope>
```

## Coding, testing, and refactoring workflows

These are defined in `.github/agents/` and can be selected in Copilot Chat by agent name.

### Feature Developer

Use for:

- New functionality and component implementation across `Gen`/`Prj` layers.
- RTE port additions, CV additions, and architecture-aligned feature work.

Good input examples:

- "Implement a new reusable utility in `Src/Gen/Util/...` and add unit tests."
- "Add a new Signal app capability and wire required RTE/CV updates."

### Test Author

Use for:

- Creating or improving unit tests (Unity/googletest adaptation).
- Adding required HAL stubs and unit-test makefile wiring.

Good input examples:

- "Add tests for `Src/Gen/Dcc/PacketExtractor` edge cases."
- "Create `Ut_<Component>` project wiring and runnable tests on win32."

### Refactoring Specialist

Use for:

- Behavior-preserving cleanup, renaming, and code-structure improvements.
- Applying style and MISRA-oriented refactors safely.

Good input examples:

- "Refactor this module to remove naming/style violations without behavior changes."
- "Extract duplicated helper logic while keeping interfaces stable."

### Suggested Combined Workflows

1. Use Feature Developer for implementation.
2. Use Test Author for coverage and test wiring.
3. Use Refactoring Specialist for targeted cleanup.

