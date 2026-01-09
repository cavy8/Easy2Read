# Changelog

All notable changes to Easy2Read will be documented in this file.

## [1.0.0] - 2026-01-09

### Added
- Initial release
- Book/note text overlay with configurable hotkey (default: F)
- Font presets: Sovngarde (default), OpenDyslexic (accessibility), ImGui Default, Custom
- Full theme configuration via `Easy2Read_Theme.ini`:
  - Title text scaling
  - Window, border, separator colors
  - Scrollbar colors, size, and rounding
  - Window rounding and padding
  - Scroll speed
- Mouse wheel scrolling support (works regardless of cursor position)
- Automatic overlay close when book menu closes
- Percentage-based window sizing (responsive to screen resolution)
- HTML/markup stripping from book text
- Pagebreak marker removal
- Leading/trailing whitespace trimming

### Technical Details
- Built with CommonLibSSE NG for SE/AE compatibility
- ImGui overlay via D3D11 hooking
- Direct vtable patching for SwapChain::Present hook
- SKSE input event capture for scroll wheel
