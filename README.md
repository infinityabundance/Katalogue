# Katalogue
Offline disk catalog for KDE Plasma CachyOS/ArchLinux — scan drives once, search them forever.

## Overview
Katalogue consists of:
- `katalogued`: a DBus-enabled daemon that scans volumes and serves results.
- `katalogue-gui`: a QML client for browsing volumes and searching files.

## Build
Dependencies:
- CMake 3.24+
- Qt 6 (Core, Sql, DBus, Quick, Qml)

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

## DBus
The daemon registers `org.kde.Katalogue1` on the session bus. You can use any
DBus tool to invoke `StartScan`, `ListVolumes`, and `SearchByName`.

## Tests
```bash
ctest --test-dir build
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

## Install & Daemon
After installing:
- GUI: `katalogue-gui`
- Daemon: `katalogued` (systemd user service)

To enable the daemon for your user:
```bash
systemctl --user enable --now katalogued.service
```
