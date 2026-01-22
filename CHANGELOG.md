# Changelog

All notable changes to Easy2Read will be documented in this file.

## [1.4.0] - 2026-01-21

### Added
- **Quest Alias Support**: `<Alias=...>` tags in book text are now resolved to actual names
  - Finds the quest that owns the book and resolves aliases from that quest
  - Reference aliases (NPCs, objects, locations) are fully supported
  - Location aliases (`BGSLocAlias`) are not yet supported due to CommonLibSSE-NG limitations
- **Enhanced Theme Customization**:
  - **Visibility Toggles** (new `[Visibility]` section in `Easy2Read_Theme.ini`):
    - `ShowTitle` - Toggle book title display
    - `ShowSeparator` - Toggle separator line under title
    - `ShowBorder` - Toggle window border
    - `ShowScrollbarTrack` - Toggle scrollbar background
  - **Per-Element Transparency** (new `[Transparency]` section):
    - `WindowAlpha` - Window background transparency (0-100)
    - `BorderAlpha` - Border transparency (0-100)
    - `SeparatorAlpha` - Separator line transparency (0-100)
    - `ScrollbarTrackAlpha` - Scrollbar track transparency (0-100)
    - `ScrollbarThumbAlpha` - Scrollbar thumb transparency (0-100)
    - Text always remains fully opaque for readability
- **Input Blocking**: Mouse and controller input is now blocked for the book menu while overlay is open
  - Prevents accidental page turning and camera movement
  - Uses MenuControls hook similar to Loading Screen Locker

### Changed
- Removed global `Opacity` setting from `[Window]` section (replaced by `WindowAlpha`)
- Alias resolution now happens before markup stripping to preserve tags

### Technical Details
- New `AliasResolver` component for quest alias resolution
- Iterates all quests to find book ownership via reference aliases
- Uses regex pattern matching for `<Alias=...>` tag detection

## [1.3.0] - 2026-01-13

### Changed
- **Compatibility Improvement**: Switched GetDescription hook from prologue hook to MinHook
  - Resolves conflicts with Dynamic String Distributor and other text modification plugins
  - MinHook creates proper trampoline chains allowing multiple plugins to hook the same function
  - Other hooks remain as call-site hooks for maximum compatibility

### Removed
- **SafeMode setting**: Removed (was added in 1.2.3 but was unnecessary)
  - Built from 1.2.2 codebase instead of 1.2.3

## [1.2.3] - 2026-01-13

### Added
- **SafeMode setting**: Limits GetDescription hook to only process book descriptions

## [1.2.2] - 2026-01-12

### Fixed
- **Tofu Remover**: Fixed crash when processing MESG (Message) records
  - MESG records are now skipped during sanitization
- **Tofu Remover**: Improved handling of unmapped characters
  - Characters without transliteration mappings now pass through unchanged instead of being replaced with `?`
  - Better support for non-English languages

### Added
- PDB file generation for debugging crash reports

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

