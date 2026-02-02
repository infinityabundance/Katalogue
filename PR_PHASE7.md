# PR: Phase 7 — Scan UX, rescan, and volume management

## Summary
This PR makes scanning a first-class UX: smarter volume identification in the
daemon, rescan behavior, and GUI scan status/controls. It also exposes active
scan state to QML with live progress updates.

## Changes
- Daemon: derive volume labels from QStorageInfo, store physical path, reuse
  volumes by fs UUID, and clear old contents before rescan.
- Core: add `findVolumeByFsUuid` and `clearVolumeContents` helpers.
- Client: track active scans, update progress/status from DBus signals, and
  refresh volumes on completion.
- QML: add Scan… folder dialog, active scan status panel, and Rescan button.

## Why
- Provides a modern scanning experience similar to VVV with live status.
- Prevents duplicate volume entries on repeated scans of the same disk.
- Enables one-click rescan using stored root paths.

## Testing
- Not run (manual daemon + GUI smoke test recommended).

## Notes
- Scan progress does not currently compute percent; it reports counts only.
- If filesystem UUID is unavailable, scans still create new volumes.
