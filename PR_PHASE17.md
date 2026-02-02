# PR: Phase 17 — Testing, QA & basic CI

## Summary
This PR adds Qt Test-based unit tests for the database and scanner, a lightweight
in-process daemon integration test, and a GitHub Actions CI workflow to build
and run tests on Linux.

## Changes
- Add tests/ CMake subdirectory and three test targets.
- Add Qt Test-based unit tests for database and scanner.
- Add in-process daemon integration test.
- Add GitHub Actions CI workflow (build + ctest).

## Why
- Provides a baseline safety net for core behaviors.
- Ensures regressions are caught early in CI.

## Testing
- Not run locally (CI will run ctest).
