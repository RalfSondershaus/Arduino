---
description: "Review arc42 docs and PlantUML diagrams for correctness, completeness, and consistency with code."
name: "Arc42 Review"
agent: "Documentation QA Reviewer"
argument-hint: "Doc or module path, e.g. docs/arc42/signal.md"
---

Perform a quality review of the specified architecture documentation scope.

## Steps

1. Validate arc42 section coverage.
2. Cross-check claims against implementation artifacts.
3. Verify diagram/text consistency.
4. Report findings by severity: CRITICAL, HIGH, MEDIUM, LOW.
5. Add open questions and concrete fix suggestions.

If no scope is provided, ask the user for a target file or module.
