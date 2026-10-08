# Changelog

All notable changes to Easy2Read will be documented in this file.

## [Unreleased]

### Fixed
- Book line breaks match the game. Vanilla book text uses CRLF line endings, and each `\r\n` was counted as two breaks, so title pages and verse showed a blank line between every line. Trailing spaces before a line break are also removed.
- Alias tags match quest alias names case-insensitively, as the game does. The engine's shared string cache keeps the first-loaded spelling (e.g. Skyrim.esm's `Questgiver`), so Missives' `<Alias=QuestGiver>` and `<Alias=Recipient>` were reported as missing from their quest.
- Stored actor/reference alias names use reference display names or base-object names; generic form-name lookup returns empty for references. Unresolved aliases now log their quest, instance and missing stored-form details at warning level.
- Open notes resolve aliases using their stored owning quest and quest instance, with stored names taking priority over live aliases. Older missives no longer depend on the quest's current run. Titles use the note's display name, and unresolved body aliases remain visible instead of disappearing.

### Added
- The mouse works in the reading overlay: a cursor appears when it opens, and the scrollbar can be clicked and dragged. Mouse clicks no longer reach the book underneath while the overlay is open.
- Optional SKSE Menu Framework 3 General and Theme pages for all INI settings, with native keyboard/controller binding pickers, grouped color pickers, font selectors, visibility controls, and sliders with units. Per-file save/reload retains comments and unknown keys; font and prompt changes refresh during play.
- Book-screen ImGui Icons button art with "Show Text", always horizontally centered on the screen and positioned vertically beneath projected book/note geometry bounds, with a lower-center fallback and `ShowBookPrompt` setting. Supports configured keyboard and Xbox controller bindings and installed icon style patches; missing artwork falls back to a text badge. Hides while the reading overlay is open.
- Location alias names and unavailable reference alias names resolve from the current quest instance's stored text data.
- PowerShell release script builds versioned mod-manager-ready main and theme zip files, with optional debug symbols and test labels.
- Modern preset preserves the previous default theme.
- Optional centered titles (`CenterTitle`) and Nordic-inspired border corners (`ShowCornerOrnaments`) in every theme configuration. Older theme files without these keys keep their original title alignment and plain border.

### Changed
- Reference aliases use custom display names before falling back to base-object names. Alias tag keywords accept any letter case, and book ownership lookup only reads forced-reference fill data for forced aliases.
- Default theme now follows SkyUI/Vanilla styling: translucent black, white titles, thin gray borders, square corners, and narrow square scrollbars. Uses bundled Barlow Condensed Regular at 24 px, with its SIL Open Font License included. Sovngarde remains available and is retained by the Modern preset.
- Reading overlay now draws after BookMenu into Skyrim's live UI framebuffer, before Community Shaders HDR/frame-generation composition. Removed swap-chain device-failure detection and cached output targets; restores graphics state after drawing.
- Updated CommonLibSSE-NG to 11.0.0 from alandtse's `ng` branch, pinned to revision `94faaed0c60eddd8347767f2d4d29a97c93bde8c`.
- Enabled SE/AE support in one DLL, including 1.5.97, 1.6.1130, and 1.7.104 with matching SKSE and Address Library.
- Renderer access now uses CommonLib's runtime data accessor; no hardcoded game addresses were added.
- Build presets use `VCPKG_ROOT`; runtime version is recorded in the plugin log.

### Removed
- Tofu Remover and its global text hooks, transliteration, and configuration settings.
- MinHook dependency and AnyASCII license, which were used only by Tofu Remover.
- Character sanitization of reading menu titles and body text.

## [1.4.1] - 2026-01-22

### Added
- **Text Cleanup**: Automatic removal of redundant formatting in book text
  - Duplicate title removal: If the first line of body text matches the book title, it's automatically removed
  - "by" pattern cleanup: Removes extra blank lines around standalone "by" author lines

### Changed
- **Scroll Position**: Overlay now always scrolls to the top when reopened

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

