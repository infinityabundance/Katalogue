# PR: Phase 2 — Implement filesystem scanner

## Summary
This PR implements the filesystem scanner used to populate the catalog. It adds
scan options (exclude patterns, hidden files, symlink handling, max depth),
progress/cancel support, and integrates scanner output with the database.

## Changes
- Add `ScanOptions`, `ScanStats`, and progress callback support.
- Implement recursive scan with exclude patterns, hidden/symlink handling, and
  max depth checks.
- Populate volume, directory, and file records using the core database API.
- Add cancel support for long-running scans.
- Extend `core_test` with a temp-tree scan validation.

## Why
- Establishes the core scanning capability needed for real datasets.
- Keeps scan behavior configurable and testable.
- Prepares for daemon and GUI integration in the next phase.

## Testing
- Not run (can run `core_test`).

## Notes
- Scanner stores paths relative to the scanned root as volume-local paths
  ("/"-prefixed).
- Excludes use simple glob matching against both relative paths and filenames.
