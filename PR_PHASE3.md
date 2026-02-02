# PR: Phase 3 — Daemon DBus API and scan jobs

## Summary
This PR implements the daemon DBus API for starting scans, tracking status, and
querying catalog data. It registers the DBus service, provides job tracking,
and exposes progress/finished signals.

## Changes
- Expand DBus interface with scan/job, volumes, and search methods.
- Implement daemon job tracking with status reporting and cancel support.
- Hook scanner progress to DBus signals.
- Add default project path under `AppDataLocation`.
- Register DBus service/object in `katalogued`.

## Why
- Establishes the background service layer required for GUI integration.
- Enables scan control and query access over DBus.

## Testing
- Not run (manual DBus smoke test recommended).

## Notes
- Job tracking is in-memory only for now.
- Search and volume listing return `a{sv}` payloads with an `items` list.
