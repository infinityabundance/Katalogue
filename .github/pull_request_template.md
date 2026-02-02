## Summary
This PR completes Phase 1 of the roadmap by hardening the SQLite core: schema
versioning, foreign keys, indices, and fully automatic FTS maintenance. It also
adds file upsert/delete APIs and expands the core test to validate
insert/rename/search/delete flows.

## Changes
- Add SQLite schema versioning (`PRAGMA user_version`) and v1 migration.
- Enforce foreign keys and cascade deletes across volumes/directories/files.
- Add indices for common lookup paths.
- Keep FTS in sync via insert/update/delete triggers and backfill logic.
- Add `upsertFile` and `deleteFile`; `insertFile` now delegates to upsert.
- Expand `core_test` to validate insert → rename → search → delete behavior.

## Why
- Ensures database evolution is safe and explicit.
- Removes manual FTS maintenance and prevents stale search results.
- Provides stable primitives for upcoming scanner + daemon work.

## Testing
- Not run (can run `core_test`).

## Notes
- This PR is self-contained and does not change scanner/daemon behavior yet.
- FTS consistency is now guaranteed by triggers, reducing application logic.
