# PR: Phase 4 — GUI integration with daemon

## Summary
This PR wires the GUI to the daemon over DBus. It introduces a lightweight
`KatalogueClient` bridge exposed to QML, updates the UI to load volumes, trigger
scans, and display search results.

## Changes
- Add `KatalogueClient` (DBus bridge) with volume/search/scan APIs.
- Register the client as a QML singleton.
- Update QML to show live volumes and search results, and provide scan/search
  inputs.
- Link GUI target with Qt DBus.

## Why
- Turns the GUI from static mock data into a usable client of the daemon.
- Enables interactive scanning and search in the UI.

## Testing
- Not run (manual GUI + daemon smoke test recommended).

## Notes
- Directory list remains placeholder pending directory-model work.
