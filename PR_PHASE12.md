# PR: Phase 12 — Export, optional VVV import, and packaging polish

## Summary
This PR adds a CLI export tool (CSV/JSON), an optional VVV import stub gated by
CMake, and installation/packaging polish. It also aligns licensing metadata and
documents new usage in the README.

## Changes
- Add `katalogue-export` CLI with volume and file export options.
- Add optional `katalogue-import-vvv` stub behind `ENABLE_VVV_IMPORT`.
- Add install() rules for binaries, desktop/metainfo, DBus XML, systemd unit, and icons (if present).
- Add `listAllFiles` helper to core for export.
- Update AppStream license metadata to match LICENSE.
- Extend README with export/import and install notes.

## Why
- Enables backup/processing of catalog data outside the GUI.
- Prepares for VVV migration without adding runtime dependencies.
- Makes installs more complete and consistent.

## Testing
- Not run (CLI smoke test and `cmake --install` recommended).
