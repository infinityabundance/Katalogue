# Katalogue

Offline disk catalog for KDE Plasma CachyOS/ArchLinux — scan drives once, search them forever.

## Features

- Scan any drive, folder, or disc into a portable `.kdcatalog` SQLite file.
- Full-text search with volume and file-type filters.
- Notes and key-value tags on files and directories.
- Virtual folders — curate custom collections across volumes.
- Multiple catalog projects; open, switch, and export at will.
- CSV and JSON export via CLI tool.
- DBus API for scripting and automation.
- Systemd user daemon with Kirigami/QtQuick GUI.
- Keyboard shortcuts: Ctrl+F (search), Ctrl+S (scan), Ctrl+O (open catalog).

## Overview

Katalogue consists of:
- `katalogued`: a DBus-enabled daemon that scans volumes and serves results.
- `katalogue-gui`: a QML client for browsing volumes and searching files.
- `katalogue-export`: a CLI tool for exporting catalog data.

## Build

Dependencies:
- CMake 3.24+
- Qt 6 (Core, Sql, DBus, Quick, Qml)
- Kirigami (for the QML GUI)

Build steps:
```bash
cmake -S katalogue -B build
cmake --build build
```

## Run

Start the daemon:
```bash
./build/katalogued
```

Launch the GUI:
```bash
./build/katalogue-gui
```

## Usage

### Creating / opening a catalog

Katalogue creates a default catalog at `~/.local/share/Katalogue/catalog.kdcatalog`. You can open a different catalog with **Ctrl+O** or the "Open catalog..." button.

### Scanning

Click **Scan...** (or press **Ctrl+S**) and select a root folder. The daemon indexes all files and directories into the current catalog. Scans run in the background and can be cancelled.

### Searching

Use the search bar (**Ctrl+F**) to find files by name. Narrow results by volume or file type. Click a result to jump to its location in the directory tree.

### Tags and notes

Select a file to open the details drawer. Add free-form notes or key-value tags (e.g., `category=docs`). Notes and tags persist in the catalog file.

### Virtual folders

Switch to the **Virtual** tab to create collections. Add files from search results or the file list using the "Add" button. Virtual folders are independent of the physical directory structure.

### Backup and restore

Your catalog is a single `.kdcatalog` file (SQLite database). Copy it to back up; open the copy to restore.

## Building and running tests

```bash
cmake -S katalogue -B build -G Ninja
cmake --build build
dbus-run-session -- ctest --test-dir build --output-on-failure
```

The daemon tests require a DBus session, so `dbus-run-session` is used as a wrapper. To skip benchmarks:

```bash
dbus-run-session -- ctest --test-dir build --output-on-failure -LE benchmark
```

## Exporting data

Katalogue provides a CLI export tool:
```bash
katalogue-export --list-volumes mycatalog.kdcatalog > volumes.csv
katalogue-export --list-files --volume-id 1 --format json mycatalog.kdcatalog > files.json
katalogue-export --list-files --search "report" mycatalog.kdcatalog > results.csv
```

## Importing from VVV (optional)

If built with `-DENABLE_VVV_IMPORT=ON`, Katalogue ships an optional importer:
```bash
katalogue-import-vvv legacy_vvv.fdb imported.kdcatalog
```
This tool is optional and not required for normal Katalogue usage.

## Install and daemon

After installing:
- GUI: `katalogue-gui`
- Daemon: `katalogued` (systemd user service)

To enable the daemon for your user:
```bash
systemctl --user enable --now katalogued.service
```

## Architecture

```
┌─────────────────────────────────────┐
│  GUI (QML / Kirigami)              │
├─────────────────────────────────────┤
│  Client Bridge (KatalogueClient)    │  ← DBus proxy
├─────────────────────────────────────┤
│  Daemon (KatalogueDaemon)           │  ← scan jobs, threading
├─────────────────────────────────────┤
│  Core Library                       │
│  ├─ KatalogueDatabase (SQLite/FTS5)│
│  └─ KatalogueScanner (filesystem)  │
├─────────────────────────────────────┤
│  Common (Settings, Types)           │
└─────────────────────────────────────┘
```

The GUI communicates with the daemon over the session DBus (`org.kde.Katalogue1`). The daemon manages the SQLite database and scanner threads.

## DBus API

The daemon exposes `org.kde.Katalogue1` on the session bus. Key methods:

```bash
# Ping
qdbus org.kde.Katalogue1 /org/kde/Katalogue1 Ping

# Start a scan
qdbus org.kde.Katalogue1 /org/kde/Katalogue1 StartScan /path/to/folder

# List volumes
qdbus org.kde.Katalogue1 /org/kde/Katalogue1 ListVolumes

# Search files
qdbus org.kde.Katalogue1 /org/kde/Katalogue1 Search "query" -1 "" 50 0
```

See `katalogue/src/daemon/org.kde.Katalogue1.xml` for the full introspection document.

## Troubleshooting

**"Database is not open"**: The daemon could not find or create the catalog file. Check that `~/.local/share/Katalogue/` is writable. You can also specify a path via `OpenProject`.

**DBus service not found**: Ensure `katalogued` is running. Start it manually (`./build/katalogued`) or enable the systemd service.

**Missing Qt/KF dependencies**: Install `qt6-base`, `qt6-declarative`, `kirigami`, and `qt6-tools` from your distro's package manager.

**Catalog locked**: Only one daemon process should access a catalog at a time. Kill any stale `katalogued` processes.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for development setup, coding conventions, and PR guidelines.

## License

Katalogue is licensed under the Apache License 2.0.
See the `LICENSE` file for details.
