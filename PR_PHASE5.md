# PR: Phase 5 — Quality, packaging, and docs

## Summary
This PR improves project usability and readiness by adding build/run docs,
registering core tests with CTest, and enriching AppStream metadata.

## Changes
- Expand README with build, run, DBus, and test guidance.
- Enable CTest and register `core_test`.
- Add AppStream metadata (licenses, homepage, developer name).

## Why
- Makes it easy to build and run the project from a clean checkout.
- Ensures tests are discoverable in CI or local workflows.
- Improves metadata completeness for packaging.

## Testing
- Not run (can run `ctest --test-dir build`).
