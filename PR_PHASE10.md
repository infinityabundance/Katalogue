# PR: Phase 10 — Metadata, tags, and annotations

## Summary
This PR adds lightweight metadata support: notes and key/value tags for files.
It extends the SQLite schema (with a migration), exposes DBus methods, and adds
an editable file details panel in the GUI.

## Changes
- Core: schema migration for notes/tags + note/tag APIs.
- Daemon: DBus methods for file notes and tags.
- GUI client: note/tag invokables for QML.
- QML: file details panel with notes and tags editor.

## Why
- Enables human annotations without heavy dependencies.
- Keeps catalogs portable and future-proof for richer metadata later.

## Testing
- Not run (manual daemon + GUI smoke test recommended).
