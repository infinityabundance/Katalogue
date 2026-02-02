# PR: Phase 16 — Internationalization, accessibility & theming

## Summary
This PR makes UI strings translatable, removes hardcoded colors, and improves
accessibility/theming consistency across the QML UI. It also adds translation
infrastructure and app metadata setup in the GUI entry point.

## Changes
- Wrap QML strings with `qsTr()` and replace hardcoded colors with theme colors.
- Wrap daemon/user-facing strings with `tr()`.
- Set app organization/name/display name and load translations (if available).
- Add translation infrastructure via `qt_add_translations` and a starter `.ts` file.

## Why
- Prepares Katalogue for localization and better theme compliance.
- Improves contrast and clarity across light/dark themes.

## Testing
- Not run (manual GUI smoke test recommended).
