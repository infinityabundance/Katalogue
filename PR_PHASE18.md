# PR: Phase 18 — Pre-release polish & 1.0 checklist

## Summary
This PR adds explicit versioning, an About page, changelog/roadmap docs, and
packaging polish. CLI tools now support --version and the GUI surfaces the app
version.

## Changes
- CMake project version + generated version header.
- CLI tools support --version.
- About page in GUI; appVersion exposed from KatalogueClient.
- CHANGELOG.md and ROADMAP.md added.
- PKGBUILD skeleton example added.
- README license section added.

## Why
- Makes Katalogue a versioned, releasable 1.0 app with clear metadata.

## Testing
- Not run (manual smoke test recommended).
