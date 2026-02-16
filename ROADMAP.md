# Katalogue Roadmap

This document lists ideas and planned directions for Katalogue after the 1.0 release.

## Short-term

- Improve performance on very large catalogs (indexing and searching).
- Optional plugin system for metadata extractors (audio, image, documents).
- More flexible tag-based and smart-collection queries.
- Better keyboard navigation and shortcuts.

## Medium-term

- Thumbnail / preview panel for supported file types (opt-in, configurable).
- Advanced export formats and scripting hooks.
- Deeper KDE integration (KIO, Baloo, etc.) where appropriate.

## Long-term

- Optional remote catalog backends (SSH / HTTP) while keeping on-disk catalog format simple.
- Shared catalog mode (read-only multi-user access).





## High-level view and assumptions
Goal assumption: Treat “completion” as taking Katalogue from a functional v1.0.0 to a robust, shippable desktop app: reliable core features (scan/search/virtual folders/tags/notes), solid tests, decent performance on large catalogs, clean UX, documented, and easy to install (at least on Linux).
## Current state (from repo analysis):
Core: Rich C++/Qt6 backend with SQLite, FTS, tags, notes, virtual folders, DBus daemon, and a Kirigami QML GUI.
Tests: Good unit/integration coverage for database, scanner, and daemon scan+search, but limited around tags/notes/virtual folders and no GUI automation.
Gaps: Incomplete VVV import, some GUI “placeholder” pieces, performance work for huge catalogs, packaging mostly PKGBUILD, and limited user/developer docs.
Phased roadmap to completion
# Phase 1 – Quality & tests foundation
Objective: Ensure core behaviors (scan, search, tags, notes, virtual folders) are tested and safe to refactor.
## Outcomes:
Critical database and daemon behaviors are regression-tested.
You can change internals with confidence.
### Microtasks:
Inventory existing tests: Map what tst_katalogue_database, tst_katalogue_scanner, and tst_katalogue_daemon already cover and log clear “coverage gaps” (tags/notes/virtual folders, schema edge cases, error paths).
Database tags/notes tests: Add tests that cover setNoteForFile, setNoteForDirectory, getNoteForFile, getNoteForDirectory, addTagToFile, tagsForFile, and removeTagFromFile.
Virtual folder tests: Add tests around createVirtualFolder, renameVirtualFolder, deleteVirtualFolder, addFileToVirtualFolder, removeFileFromVirtualFolder, listVirtualFolders, and listVirtualFolderItems.
Project stats and listAllFiles tests: Verify projectStats and listAllFiles on non-trivial catalogs (multiple volumes, directories, file types).
Daemon API edge tests: Extend tst_katalogue_daemon with failure cases (invalid project paths, failed scans, search on empty catalogs).
Test docs: Add a short “How to build & run tests” section to the README (build dir, ctest invocation, expected test set).
# Phase 2 – UX, GUI, and feature completeness
## Objective: Make the GUI experience polished and close functional gaps.
### Outcomes:
Directory tree and virtual folders behave intuitively.
Core tagging/notes workflows are discoverable in the GUI.
### Microtasks:
Audit current QML views: Identify where tree navigation, tags, notes, and virtual folders are exposed or missing.
Directory tree refinement: Replace any “placeholder” list-only behavior with a proper collapsible tree (volumes → directories).
Tags/notes UI: Ensure you can add/edit/remove notes and tags from file detail views and see them reflect instantly.
Virtual folder UI: Expose create/rename/delete folders and drag/drop (or action-based) assignment of files.
Empty/error states: Design and implement clear states for empty catalogs, failed scans, and search with no results.
Keyboard shortcuts & accessibility: Add at least basic shortcuts (search focus, start scan, open details) and ensure navigability without the mouse.
# Phase 3 – Performance & scalability
## Objective: Handle large catalogs (hundreds of thousands to millions of files) without feeling sluggish.
### Outcomes:
Scans don’t lock up the UI.
Searches and tree expansion remain responsive.
### Microtasks:
Baseline performance measurements: Script a simple synthetic catalog (or use real data) to profile scan time, search latency, and DB size.
Scanner tuning: Adjust scan options (batching, depth, hidden files, I/O patterns) and thread usage to keep UI responsive.
Database indexing review: Revisit schema and indexes for files, directories, file_fts, notes, and tags for typical queries (search, browse, tags).
Pagination & lazy loading: Add paging in heavy views (search results, large directories, big virtual folders); load more on demand.
Caching layer: Introduce lightweight caches for hot data (recent searches, last-opened directories) while maintaining correctness.
Performance regression tests: Add simple performance “guards” (timing thresholds or at least benchmark scripts) to catch accidental slowdowns.
# Phase 4 – Packaging, installation, and releases
## Objective: Make Katalogue easy to install and keep up to date on target platforms.
Outcomes:
Simple install path for a “typical” Linux user.
Repeatable release process.
### Microtasks:
Harden PKGBUILD: Ensure the provided PKGBUILD is up to date with current dependencies and build steps.
AppImage or Flatpak: Create a cross-distro package with desktop, DBus, and systemd-user integration.
Versioning and changelog: Adopt a simple versioning scheme (e.g., semver) and maintain CHANGELOG.md.
Release automation: Add CI jobs that build artifacts for tagged releases (including tests before publish).
Smoke-test checklist: Document a quick manual “release smoke-test” (scan sample folders, search, tags/notes, virtual folders).
# Phase 5 – Documentation and onboarding
## Objective: Make it easy for both users and contributors to understand, use, and modify Katalogue.
### Outcomes:
Clear user path from install → first catalog → advanced use.
Clear contributor path from clone → build → test → PR.
### Microtasks:
User guide: Expand README into sections for install, create/open catalog, scan, search, tags/notes, virtual folders, backup/restore.
Developer guide: Add architecture overview (core DB, scanner, daemon, GUI), build instructions, testing instructions, and coding conventions.
DBus & DB docs: Briefly document DBus API and schema (or generate from existing XML/DDL).
Troubleshooting section: Common errors (DB corruption, missing Qt/KF deps, DBus issues) and remedies.
Phase 6 – Maintenance, tooling, and future features
## Objective: Keep the project healthy and ready for long-term evolution.
### Outcomes:
Issues and PRs are easy to triage.
Foundation for future features (like VVV import, remote catalogs).
### Microtasks:
Issue labels & templates: Add GitHub issue templates (bug, feature, question) and a simple labeling scheme.
PR guidelines: Define expectations for tests, style, and documentation in CONTRIBUTING.
Static checks: Optionally add clang-tidy/cppcheck/QML lints into CI (non-blocking at first).
Roadmap doc: Turn the implicit roadmap (VVV import, remote catalogs, perf) into a short ROADMAP.md.
What I’ve already executed (Phase 1 microtasks)
Test inventory: I reviewed the existing tests in katalogue/tests:
tst_katalogue_database already covers opening projects, inserting volumes/paths, and basic search.
tst_katalogue_scanner covers scanning directory trees, hidden files behavior, and max depth.
tst_katalogue_daemon runs a full scan via the daemon and validates search results.
New database test for notes & tags:
I updated tst_katalogue_database.cpp to add a new slot testNotesAndTags() which:
Opens a temporary project, creates a volume/root directory/file.
Sets and reads back file notes and directory notes using setNoteForFile, getNoteForFile, setNoteForDirectory, getNoteForDirectory.
Adds multiple tags to a file, verifies they are present via tagsForFile, then removes one via removeTagFromFile and asserts it is gone.
This directly exercises the notes/tags/file_tags schema and the related API surface on KatalogueDatabase.
Tests and build commands:
I attempted to configure and build the project with cmake -S katalogue -B build && cmake --build build && ctest --test-dir build.
CMake currently fails on existing katalogue-gui/translations/Kirigami target/linking issues (mixed keyword/plain target_link_libraries, missing katalogue_translations target, Kirigami QML plugin targets), which are unrelated to the new test.
You can use these same commands locally; once the CMake GUI/translation configuration issues are resolved in your environment, the new testNotesAndTags test should build and run along with the rest of the suite.
How to proceed next
If you like this roadmap, I can next start on Phase 1’s remaining core coverage—for example, adding tests for virtual folders and projectStats/listAllFiles, then move on to a concrete Phase 2 GUI microtask (e.g., tightening up the directory tree behavior).
If you’d rather narrow scope (e.g., “focus only on performance and packaging” or “prioritize GUI polish first”), tell me your priorities and target platforms and I’ll reorder and deepen the relevant phases.
