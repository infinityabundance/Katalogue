# PR: Phase 6 — Physical view (directory + file browsing)

## Summary
This PR implements the physical browse view: selecting a volume shows its
root directories, selecting a directory shows its child directories and files.
It extends the core DB API, daemon DBus interface, GUI client, and QML wiring.

## Changes
- Core: add directory/file listing and directory lookup APIs.
- Daemon: expose `ListDirectories` and `ListFiles` over DBus.
- Client: track selected volume/directory and load entries via DBus.
- QML: wire volume selection → directory list → file list.

## Why
- Enables offline browsing of catalog data, matching the intended UX.

## Testing
- Not run (manual daemon + GUI smoke test recommended).

## Notes
- Root directories are resolved using `parent_id` = -1 or NULL.
- File view shows current directory contents; search is still available via the
  search action.
