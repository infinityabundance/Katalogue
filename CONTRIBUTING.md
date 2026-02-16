# Contributing to Katalogue

Thank you for your interest in contributing to Katalogue.

## Getting started

### Prerequisites

- CMake 3.24+
- Qt 6 (Core, Sql, DBus, Quick, Qml, Test)
- Kirigami
- Ninja (recommended) or Make
- A DBus session (for running daemon tests)

### Building

```bash
git clone https://github.com/infinityabundance/Katalogue.git
cd Katalogue
cmake -S katalogue -B build -G Ninja
cmake --build build
```

### Running tests

```bash
dbus-run-session -- ctest --test-dir build --output-on-failure
```

To skip long-running benchmarks:

```bash
dbus-run-session -- ctest --test-dir build --output-on-failure -LE benchmark
```

### Running the app

Start the daemon and GUI in separate terminals:

```bash
./build/katalogued
./build/katalogue-gui
```

## Architecture

Katalogue follows a layered architecture:

1. **Core library** (`katalogue-core`): `KatalogueDatabase` (SQLite + FTS5) and `KatalogueScanner` (filesystem walker). No Qt GUI or DBus dependencies.
2. **Daemon** (`katalogued`): Exposes the core via `org.kde.Katalogue1` DBus service. Manages scan jobs on a background thread.
3. **GUI client** (`katalogue-gui`): QML/Kirigami app that communicates with the daemon over DBus via `KatalogueClient`.
4. **Common**: Shared types (`katalogue_types.h`) and settings (`KatalogueSettings`).

### Key files

| Component | Files |
|-----------|-------|
| Database  | `src/core/katalogue_database.h/.cpp` |
| Scanner   | `src/core/katalogue_scanner.h/.cpp` |
| Types     | `src/core/katalogue_types.h` |
| Daemon    | `src/daemon/katalogue_daemon.h/.cpp` |
| GUI Client| `src/gui/katalogue_client.h/.cpp` |
| QML Views | `src/gui/qml/*.qml` |
| Tests     | `tests/tst_katalogue_*.cpp` |

## Coding conventions

- C++20, Qt 6 style.
- Use `QStringLiteral` for string literals.
- Use `std::optional` for nullable return values.
- Prefer `prepare`/`addBindValue` over string interpolation in SQL.
- Header guards: `#pragma once`.
- Test new database or scanner features with QtTest.

## Pull request guidelines

Before submitting a PR:

1. All existing tests pass (`ctest --output-on-failure`).
2. New features include corresponding tests.
3. Code compiles without warnings (`-Wall -Wextra` where practical).
4. PR description explains what changed and why.
5. If the PR affects the GUI, include a brief description of visual changes.

## Reporting issues

Use the GitHub issue tracker. Please include:

- Steps to reproduce.
- Expected vs. actual behavior.
- Katalogue version, Qt version, and OS/distro.
- Relevant log output (run `katalogued` or `katalogue-gui` from a terminal to see warnings).
