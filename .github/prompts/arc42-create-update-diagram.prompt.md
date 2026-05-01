---
description: "Generate or update PlantUML diagrams that support arc42 views for a target module."
name: "Arc42 Diagram"
agent: "PlantUML Diagram Author"
argument-hint: "Target module and diagram type, e.g. Src/Prj/App/Signal runtime"
---

Create or update PlantUML diagrams for the requested module and viewpoint.

## Steps

1. Determine required view: building block, runtime, deployment, or class relations.
2. Read source/docs to verify names and data flow.
3. Write/update `.puml` files in `docs/components/`.
4. Ensure labels match implementation symbols and architecture terms.
5. Add short assumptions notes in comments if needed.

If no viewpoint is specified, default to building block + runtime.
