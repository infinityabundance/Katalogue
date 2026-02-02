# PR: Phase 11 — Virtual folders (logical collections)

## Summary
This PR adds virtual folders: a logical tree independent of physical volumes.
Users can create collections, add/remove files, and jump from virtual items to
physical locations.

## Changes
- Core: schema migration for virtual folders/items + CRUD APIs.
- Daemon: DBus methods for listing/creating/renaming/deleting folders and items.
- Client: virtual folder state, selection, and item management.
- QML: virtual view tab with folders and items, add/remove actions from file and search lists.

## Why
- Enables cross-disk collections without moving files.
- Provides a VVV-style virtual view with modern architecture.

## Testing
- Not run (manual daemon + GUI smoke test recommended).
