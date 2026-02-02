# PR: Phase 8 — Project management and multiple catalogs

## Summary
This PR makes project files first-class: the daemon exposes project info over
DBus, the core provides project stats, and the GUI can open catalogs and show
current project metadata.

## Changes
- Core: add `ProjectStats` and `projectStats()` aggregate query helper.
- Daemon: OpenProject returns a{sv} info map and GetProjectInfo added.
- GUI client: projectPath/projectInfo properties, openProject + refreshProjectInfo.
- QML: Open catalog dialog and project info display.

## Why
- Enables multi-catalog workflows and makes the current project explicit.
- Surfaces basic stats to give immediate context for a catalog.

## Testing
- Not run (manual daemon + GUI smoke test recommended).
