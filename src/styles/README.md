# Style Architecture

This folder centralizes all runtime Qt UI styling for performance-critical workflows.

## Layering Strategy

1. `QPalette` (`ThemeManager::buildPalette`)
- Global colors and contrast roles.
- Fast lookups (`O(1)` role fetch).

2. `QProxyStyle` (`PerformanceProxyStyle`)
- Structural metrics (frame width, spacing, button margins, scrollbar extent).
- Avoids deep QSS for geometry behavior.

3. Scoped QSS (`qss/*.qss`)
- `application.qss`: lightweight global chrome (menu/status/tooltip).
- `activity_rail.qss`: activity rail only (`QToolBar#ActivityRail`).
- `welcome_page.qss`: welcome page only (ID selectors).

## Theme Set

- `Light Lavender` accent `#7C4DFF`
- `Dark Green Blue` accent `#3FB950`
- `Dark Orange Purple` accent `#FF6B35`

## Performance Rules

- Keep selectors shallow (type or ID selectors only).
- Avoid nested descendant chains.
- Prefer palette roles (`palette(...)`) over per-theme QSS duplication.
- Load QSS once from resources and reuse cached strings.
- Apply styles only where needed (scoped widget-level QSS for heavy screens).
