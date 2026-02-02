## What
- Add SQLite schema versioning and v1 migration.
- Enforce foreign keys and add cascade deletes.
- Add indices for common lookup paths.
- Keep FTS in sync via insert/update/delete triggers and backfill.
- Add `upsertFile` + `deleteFile`; `insertFile` delegates to upsert.
- Expand core test to cover insert/rename/search/delete.

## Why
- Ensures database evolution is safe and explicit.
- Keeps search results correct without manual FTS maintenance.
- Prepares core for real scanner integration and daemon usage.

## Testing
- Not run (can run `core_test`).
