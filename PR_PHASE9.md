# PR: Phase 9 — Advanced search and jump-to-folder

## Summary
This PR adds a filtered search API and integrates search results with the
physical view. Users can filter by volume/type, see richer results, and jump to
the containing folder with one click.

## Changes
- Core: extend SearchResult and add filtered `search(...)` API.
- Daemon: add DBus `Search` method returning rich results.
- Client: new `search(...)` and `jumpToResult(...)` helpers.
- QML: search panel with volume/type filters, results list, and jump behavior.

## Why
- Enables scoped, richer search without losing the physical browsing context.
- Makes search results actionable by navigating to the file’s directory.

## Testing
- Not run (manual daemon + GUI smoke test recommended).
