# PR: Phase 15 — Settings & preferences

## Summary
This PR adds a shared QSettings backend, wires scanner defaults into the daemon,
and introduces a Preferences page for scanner/UI options.

## Changes
- Add `KatalogueSettings` helper (scanner + UI prefs).
- Daemon StartScan now uses persisted scanner settings.
- GUI exposes settings properties and a Preferences page.
- Virtual folder delete confirmation respects a UI setting.
- Scanner options extended with computeHashes (stored, not yet used).

## Why
- Makes scan behavior consistent and user-configurable.
- Provides a simple, KDE-style preferences UI.

## Testing
- Not run (manual GUI + scan smoke test recommended).
