# PR: Phase 13 — Core & daemon hardening

## Summary
This PR hardens schema validation, scan cancellation, and daemon error handling.
Catalogs now have explicit schema_info versions, scans can be cancelled safely,
and DBus methods return clearer failures when the DB is unavailable.

## Changes
- Core: schema_info table/version checks, lastErrorString, and stricter openProject validation.
- Scanner: explicit cancel request handling and skipping of symlinks/special files.
- Daemon: init() with schema validation, DBus error replies, and clearer scan status/errors.
- Main: fail fast if daemon init fails before registering DBus.

## Why
- Prevents silent corruption and mismatched schema issues.
- Makes scan lifecycle robust and user-cancellable.
- Improves daemon reliability and diagnostic clarity.

## Testing
- Not run (manual daemon/scan smoke test recommended).
