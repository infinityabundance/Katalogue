# Katalogue Roadmap

This document lists ideas and planned directions for Katalogue after the 1.0 release.

## High-level goals

Take Katalogue from a functional v1.0.0 to a robust, shippable desktop app: reliable core features (scan/search/virtual folders/tags/notes), solid tests, decent performance on large catalogs, clean UX, documentation, and easy installation (at least on Linux).

## Current state

- **Core:** Rich C++/Qt6 backend with SQLite, FTS, tags, notes, virtual folders, DBus daemon, and a Kirigami QML GUI.
- **Tests:** Good unit/integration coverage for database, scanner, and daemon scan+search, but limited around tags/notes/virtual folders and no GUI automation.
- **Gaps:** Incomplete VVV import, some GUI placeholder pieces, performance work for huge catalogs, packaging mostly PKGBUILD, and limited user/developer docs.

---

## Phase 1 – Quality & tests foundation

**Objective:** Ensure core behaviors (scan, search, tags, notes, virtual folders) are tested and safe to refactor.

**Outcomes:**
- Critical database and daemon behaviors are regression-tested.
- Internals can be changed with confidence.

**Microtasks:**
- Inventory existing tests: map what `tst_katalogue_database`, `tst_katalogue_scanner`, and `tst_katalogue_daemon` already cover and log clear coverage gaps (tags/notes/virtual folders, schema edge cases, error paths).
- Database tags/notes tests: add tests covering `setNoteForFile`, `setNoteForDirectory`, `getNoteForFile`, `getNoteForDirectory`, `addTagToFile`, `tagsForFile`, and `removeTagFromFile`.
- Virtual folder tests: add tests around `createVirtualFolder`, `renameVirtualFolder`, `deleteVirtualFolder`, `addFileToVirtualFolder`, `removeFileFromVirtualFolder`, `listVirtualFolders`, and `listVirtualFolderItems`.
- Project stats and `listAllFiles` tests: verify `projectStats` and `listAllFiles` on non-trivial catalogs (multiple volumes, directories, file types).
- Daemon API edge tests: extend `tst_katalogue_daemon` with failure cases (invalid project paths, failed scans, search on empty catalogs).
- Test docs: add a short "How to build & run tests" section to the README (build dir, ctest invocation, expected test set).

## Phase 2 – UX, GUI, and feature completeness

**Objective:** Make the GUI experience polished and close functional gaps.

**Outcomes:**
- Directory tree and virtual folders behave intuitively.
- Core tagging/notes workflows are discoverable in the GUI.

**Microtasks:**
- Audit current QML views: identify where tree navigation, tags, notes, and virtual folders are exposed or missing.
- Directory tree refinement: replace any placeholder list-only behavior with a proper collapsible tree (volumes → directories).
- Tags/notes UI: ensure adding/editing/removing notes and tags from file detail views reflects instantly.
- Virtual folder UI: expose create/rename/delete folders and drag/drop (or action-based) assignment of files.
- Empty/error states: design and implement clear states for empty catalogs, failed scans, and search with no results.
- Keyboard shortcuts & accessibility: add at least basic shortcuts (search focus, start scan, open details) and ensure navigability without the mouse.

## Phase 3 – Performance & scalability

**Objective:** Handle large catalogs (hundreds of thousands to millions of files) without feeling sluggish.

**Outcomes:**
- Scans don't lock up the UI.
- Searches and tree expansion remain responsive.

**Microtasks:**
- Baseline performance measurements: script a synthetic catalog (or use real data) to profile scan time, search latency, and DB size.
- Scanner tuning: adjust scan options (batching, depth, hidden files, I/O patterns) and thread usage to keep UI responsive.
- Database indexing review: revisit schema and indexes for files, directories, `file_fts`, notes, and tags for typical queries (search, browse, tags).
- Pagination & lazy loading: add paging in heavy views (search results, large directories, big virtual folders); load more on demand.
- Caching layer: introduce lightweight caches for hot data (recent searches, last-opened directories) while maintaining correctness.
- Performance regression tests: add simple performance guards (timing thresholds or benchmark scripts) to catch accidental slowdowns.

## Phase 4 – Packaging, installation, and releases

**Objective:** Make Katalogue easy to install and keep up to date on target platforms.

**Outcomes:**
- Simple install path for a typical Linux user.
- Repeatable release process.

**Microtasks:**
- Harden PKGBUILD: ensure the provided PKGBUILD is up to date with current dependencies and build steps.
- AppImage or Flatpak: create a cross-distro package with desktop, DBus, and systemd-user integration.
- Versioning and changelog: adopt a simple versioning scheme (e.g., semver) and maintain CHANGELOG.md.
- Release automation: add CI jobs that build artifacts for tagged releases (including tests before publish).
- Smoke-test checklist: document a quick manual release smoke-test (scan sample folders, search, tags/notes, virtual folders).

## Phase 5 – Documentation and onboarding

**Objective:** Make it easy for both users and contributors to understand, use, and modify Katalogue.

**Outcomes:**
- Clear user path from install → first catalog → advanced use.
- Clear contributor path from clone → build → test → PR.

**Microtasks:**
- User guide: expand README into sections for install, create/open catalog, scan, search, tags/notes, virtual folders, backup/restore.
- Developer guide: add architecture overview (core DB, scanner, daemon, GUI), build instructions, testing instructions, and coding conventions.
- DBus & DB docs: briefly document DBus API and schema (or generate from existing XML/DDL).
- Troubleshooting section: common errors (DB corruption, missing Qt/KF deps, DBus issues) and remedies.

## Phase 6 – Maintenance, tooling, and future features

**Objective:** Keep the project healthy and ready for long-term evolution.

**Outcomes:**
- Issues and PRs are easy to triage.
- Foundation for future features (VVV import, remote catalogs).

**Microtasks:**
- Issue labels & templates: add GitHub issue templates (bug, feature, question) and a simple labeling scheme.
- PR guidelines: define expectations for tests, style, and documentation in CONTRIBUTING.
- Static checks: optionally add clang-tidy/cppcheck/QML lints into CI (non-blocking at first).

---

## Long-term ideas

- Optional plugin system for metadata extractors (audio, image, documents).
- More flexible tag-based and smart-collection queries.
- Thumbnail / preview panel for supported file types (opt-in, configurable).
- Advanced export formats and scripting hooks.
- Deeper KDE integration (KIO, Baloo, etc.) where appropriate.
- Optional remote catalog backends (SSH / HTTP) while keeping on-disk catalog format simple.
- Shared catalog mode (read-only multi-user access).
