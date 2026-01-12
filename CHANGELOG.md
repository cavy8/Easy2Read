# Changelog

All notable changes to Easy2Read will be documented in this file.

## [1.2.1] - 2026-01-12

### Fixed
- **D3D12 Compatibility**: Fixed overlay rendering when Community Shaders frame generation is enabled
  - Overlay now renders correctly on top of book content with D3D12 swap chain proxy
  - Uses currently bound render target from device context in D3D12 fallback mode
- **Tofu Remover**: Improved angle bracket handling
  - Content inside `<>` brackets (e.g., `<Alias=Player>`) is now preserved as-is
  - Only preserves bracket content when both opening and closing brackets exist

## [1.2.0] - 2026-01-10

### Added
- **Controller Support**: Full gamepad support for overlay
  - Configurable toggle button (default: Y button on Xbox)
  - Right thumbstick for scrolling through text
  - Adjustable scroll speed via `ControllerScrollSpeed` setting
- New INI settings in `[General]`:
  - `EnableOverlay` - Master toggle to disable overlay feature entirely
  - `ControllerToggleButton` - Configurable gamepad button for toggle
  - `ControllerScrollSpeed` - Scroll speed for controller thumbstick

### Changed
- Renamed text sanitization mode from `AnyASCII` to `On` for clarity
- Mode options are now: `On`, `DetectOnly`, or `Off`

## [1.1.0] - 2026-01-10

### Added
- **Tofu Remover**: Automatic text sanitization to replace unsupported Unicode characters
  - Hooks for: descriptions, dialogue subtitles, dialogue menu, quest journal
  - Detection-only for: map markers, NPC names
  - CP1252 (Windows-1252) character handling for common in-game text
- New INI settings in `[TextSanitization]`:
  - `Enable` - Master toggle
  - `DebugMode` - Verbose logging for troubleshooting
  - `LogReplacements` - Log each character replacement
- New INI section `[TextSanitization.Hooks]` for per-hook enable/disable

### Technical Details
- Based on Dynamic String Distributor hooking approach
- Uses AnyASCII-style transliteration for Unicode → ASCII conversion
- Character set from Tofu Detective for compatibility detection

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
- Image-to-text mappings support for mods like Scribes of Skyrim

### Technical Details
- Built with CommonLibSSE NG for SE/AE compatibility
- ImGui overlay via D3D11 hooking
- Direct vtable patching for SwapChain::Present hook
- SKSE input event capture for scroll wheel

