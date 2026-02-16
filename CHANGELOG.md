# Katalogue Changelog

## [1.1.0] - 2026-02-16

### Tests
- Added comprehensive database tests for notes, tags, and virtual folders.
- Added project stats and `listAllFiles` tests with multi-volume catalogs.
- Added daemon edge-case tests (empty paths, invalid IDs, empty catalogs).
- Added scanner tests for exclude patterns and non-existent paths.

### GUI / UX
- Directory tree now supports drill-down navigation with a back button and path breadcrumb.
- Virtual folders can now be renamed from the sidebar.
- Added keyboard shortcuts: Ctrl+F (search), Ctrl+S (scan), Ctrl+O (open catalog), Escape (clear search).
- File sizes now display in human-readable format (KB/MB/GB) instead of raw bytes.
- Added empty-state messages for virtual folder items, directory tree, and search results.
- Search results now support pagination with a "Load more" button.

### Performance
- Scanner now uses transaction batching (500 files per batch) for significantly faster scans.
- Added `beginBatch()`/`endBatch()` API to `KatalogueDatabase` for bulk operations.
- Added performance benchmark (`bench_catalog`) for measuring insert, search, and list performance.

### Packaging & CI
- Hardened `PKGBUILD.example` with `check()` function, Kirigami dependency, and `CMAKE_INSTALL_PREFIX`.
- Added Flatpak manifest (`packaging/org.kde.katalogue.yml`).
- Added CI release automation: tagged pushes create GitHub releases with build artifacts.
- Added cppcheck static analysis job to CI (non-blocking).
- Bumped version to 1.1.0.

### Documentation
- Expanded README with features overview, user guide sections, architecture overview, DBus API examples, troubleshooting, and test instructions.
- Added CONTRIBUTING.md with developer guide, build instructions, and PR expectations.
- Added GitHub issue templates (bug report, feature request) and PR template.

## [1.0.0] - 2026-02-02

- Initial 1.0 release.
- Modern Qt 6 / KDE Plasma disk catalog.
- Features:
  - Drive / disc scanning into `.kdcatalog` SQLite projects.
  - Offline search with filters and "jump to folder".
  - Metadata: notes, tags, and virtual folders.
  - Multiple projects support and export (CSV/JSON).
  - Optional VVV import tool (build-time opt-in).
  - Systemd user daemon + DBus API, QtQuick/Kirigami GUI.
