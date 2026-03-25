# Easy2Read

An SKSE plugin for Skyrim SE/AE that displays book and note text in a custom overlay window with configurable fonts for improved readability, plus a universal text sanitizer ("Tofu Remover") that fixes unsupported Unicode characters in dialogue, books, etc.

## Features

When reading a book/note, press the F key (configurable) to pull up an overlay with the text. Theme support, multiple font options (including the OpenDyslexic font), and includes support for image descriptions/text replacement. By default, mappings are provided for vanilla and Scribes of Skyrim calligraphy. Certain aliases still don't convert to text properly and will simply be skipped over. If you have an idea on how to fix this, please reach out or submit a PR

### Tofu Remover (Text Sanitizer)
Automatically replaces unsupported Unicode characters ("tofu" □) with readable ASCII equivalents in books, dialogue, and quest records. Map markers, NPC names, and loading screens are currently not supported. Loading screen tips use a different system that seems to crash when hooked the same way as the other text. As such, I've not implemented a hook for it. This is a known limitation at this time. If you have a solution for this, please reach out or submit a PR :)

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
```

### Easy2Read_Theme.ini

Customize the overlay appearance:

- **[Font]**: FontPreset (Sovngarde/Dyslexic/ImGui/Custom), FontSize, TitleScale
- **[Colors]**: Title, body text, window, border, separator colors (RGB 0-255)
- **[Scrollbar]**: Background, thumb, hover colors, size, rounding, scroll speed
- **[Window]**: Size (% of screen), opacity, rounding, padding

### Foreign Language Support

- Supports English and some European languages by default.
- To use with other languages, you must edit the Easy2Read_Theme.ini file and provide a custom font, and set the LanguageSupport value to reflect your desired language. Small memory impact if using "Full" - I recommend a more specialized preset if possible.

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

## Future Plans

- Fix loading screens if possible
- Show the key to press for the overlay in the book UI
- Better foreign language support

## License

This project is licensed under GPL-3.0. Sovngarde and OpenDyslexic font files are licensed under the SIL Open Font License. Futura font file is from dafontfamily.com, license not specified.

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
- Paul Renner for Futura font
