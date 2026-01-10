# Easy2Read

An SKSE plugin for Skyrim SE/AE that displays book and note text in a custom overlay window with configurable fonts for improved readability, plus a universal text sanitizer ("Tofu Remover") that fixes unsupported Unicode characters in dialogue, books, etc.

## Features

### Book Overlay
- **Customizable Overlay**: Display book text in a clean, readable overlay
- **Font Options**: Choose from Sovngarde (Skyrim-themed), OpenDyslexic (accessibility), or custom fonts
- **Full Theming**: Configure colors, opacity, scrollbar, borders, and more
- **Scroll Wheel Support**: Scroll through long texts with your mouse wheel
- **Hotkey Toggle**: Press a configurable key to show/hide the overlay while reading

### Tofu Remover (Text Sanitizer)
Automatically replaces unsupported Unicode characters ("tofu" □) with readable ASCII equivalents:

- **Books, Items, Spells** - DESC/CNAM records
- **Dialogue Subtitles** - INFO NAM1 records
- **Dialogue Menu Options** - DIAL FULL records  
- **Quest Journal Descriptions** - QUST CNAM records
- **Map Markers** - Detection only (REFR FULL)
- **NPC Names** - Detection only (NPC FULL)

> **Note**: Loading screen tips use a different system (Scaleform) that seems to crash when hooked the same way as the other text. As such, I've not implemented a hook for it. This is a known limitation at this time.

## Installation

1. Install [SKSE](https://skse.silverlock.org/) for your Skyrim version
2. Copy the `Data/SKSE/Plugins/Easy2Read` directory to your Skyrim installation's `Data/SKSE/Plugins/` directory
3. ???
4. Profit

## Usage

### Book Overlay
1. Open any book or note in Skyrim
2. Press the toggle key (default: **F**) to display the overlay
3. Scroll with your mouse wheel to read long texts
4. Press the toggle key again or close the book to hide the overlay

### Tofu Remover
Works automatically! Unsupported characters are replaced as text loads.

## Configuration

### Easy2Read.ini

```ini
[General]
ToggleKey = 33  ; Hotkey scancode (default F = 33)

[TextSanitization]
Enable = true           ; Master toggle for Tofu Remover
Mode = On               ; On, DetectOnly, or Off
DebugMode = false       ; Verbose logging for troubleshooting
LogReplacements = false ; Log each character replacement

[TextSanitization.Hooks]
; Disable individual hooks if they cause issues
EnableDescriptionHook = true    ; Books, items, spells
EnableDialogueHook = true       ; Dialogue subtitles
EnableDialogueMenuHook = true   ; Dialogue menu options
EnableQuestHook = true          ; Quest journal
EnableMapMarkerHook = true      ; Map markers (detect only)
EnableNpcNameHook = true        ; NPC names (detect only)
```

### Easy2Read_Theme.ini

Customize the overlay appearance:

- **[Font]**: FontPreset (Sovngarde/Dyslexic/ImGui/Custom), FontSize, TitleScale
- **[Colors]**: Title, body text, window, border, separator colors (RGB 0-255)
- **[Scrollbar]**: Background, thumb, hover colors, size, rounding, scroll speed
- **[Window]**: Size (% of screen), opacity, rounding, padding

## Building from Source

### Requirements

- Visual Studio 2022 with "Desktop development with C++" workload
- CMake 3.20+
- vcpkg with `VCPKG_ROOT` environment variable set

### Build Steps

```powershell
# Clone the repository
git clone <repository-url>
cd Easy2Read

# Configure with CMake (vcpkg will fetch dependencies)
cmake --preset default

# Build
cmake --build build-vs --config Release
```

The built DLL will be in `build-vs/Release/Easy2Read.dll`.

## Dependencies

- [CommonLibSSE NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) - SKSE plugin framework
- [ImGui](https://github.com/ocornut/imgui) - Overlay rendering
- [SimpleIni](https://github.com/brofield/simpleini) - INI file parsing

## Known Limitations

1. **VR Untested**: Built to target VR but not tested. Please report if it works!
2. **Loading Screens**: Cannot sanitize loading screen tips (Scaleform limitation)
3. **Multi-page Books**: All text shown concatenated; page breaks not preserved
4. **Dynamic Text**: Books with script-generated text may show base text in overlay.
5. **Input Capturing**: Scroll wheel input still passes to underlying book menu
6. **Map Markers/NPC Names**: Detection only - cannot modify these in-place at this time.

## License

This project is licensed under GPL-3.0. Font files are licensed under the SIL Open Font License.

I can't stop you from doing anything you want with this. That said, I'd still appreciate it if you reached out to me first :)

## Credits

- CommonLibSSE NG team for the SKSE framework
- ImGui for the immediate-mode GUI library
- SSE-ImGui project for D3D11 hooking reference
- AnyASCII for transliteration table
- SkyHorizon3 for Dynamic String Distributor reference
- OpenDyslexic font
- mjorka for Sovngarde font
- Community Shaders team (input reference)
- krypto5863 for Tofu Detective (character set reference)

