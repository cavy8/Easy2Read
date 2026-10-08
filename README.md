# Easy2Read

An SKSE plugin for Skyrim SE/AE that displays book and note text in a custom overlay window with configurable fonts for improved readability.

## Features

When reading a book/note, press the F key (configurable) to pull up an overlay with the text. Theme support, multiple font options (including the OpenDyslexic font), and includes support for image descriptions/text replacement. By default, mappings are provided for vanilla and Scribes of Skyrim calligraphy. Certain aliases still don't convert to text properly and will simply be skipped over. If you have an idea on how to fix this, please reach out or submit a PR

## Installation

1. Install [SKSE](https://skse.silverlock.org/) for your Skyrim version
2. Install [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444) for your game version (SE for 1.5.x; AE for 1.6.x/1.7.x).
3. Install `Easy2Read.dll` in `Data/SKSE/Plugins/` and copy the supplied `Data` contents into your game's `Data` directory.

### Runtime compatibility

One DLL targets Skyrim SE 1.5.97 and AE 1.6.x/1.7.x, including 1.6.1130,
1.6.1170, GOG 1.6.1179, and 1.7.104. Each runtime needs its matching SKSE and
Address Library.

## Usage

### Book Overlay
1. Open any book or note in Skyrim
2. Press the toggle key (default: **F**) to display the overlay
3. Scroll with your mouse wheel to read long texts
4. Press the toggle key again or close the book to hide the overlay

## Configuration

### Easy2Read.ini

```ini
[General]
ToggleKey = 33  ; Hotkey scancode (default F = 33)
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
- CMake 3.25+ (for the version 6 presets)
- Git (CMake downloads the pinned CommonLibSSE-NG source)
- vcpkg with `VCPKG_ROOT` environment variable set

### Build Steps

```powershell
# Clone the repository
git clone <repository-url>
cd Easy2Read

# Configure with CMake (vcpkg will fetch dependencies)
cmake --preset default

# Build
cmake --build --preset release
```

The built DLL will be in `build/Release/Easy2Read.dll`.

## Dependencies

- [CommonLibSSE NG](https://github.com/alandtse/CommonLibSSE-NG/tree/ng) - SKSE plugin framework
- [ImGui](https://github.com/ocornut/imgui) - Overlay rendering
- [SimpleIni](https://github.com/brofield/simpleini) - INI file parsing

## Future Plans

- Show the key to press for the overlay in the book UI
- Better foreign language support

## License

This project is licensed under GPL-3.0. Sovngarde and OpenDyslexic font files are licensed under the SIL Open Font License. Futura font file is from dafontfamily.com, license not specified.

I can't stop you from doing anything you want with this. That said, I'd still appreciate it if you reached out to me first :)

## Credits

- CommonLibSSE NG team for the SKSE framework
- ImGui for the immediate-mode GUI library
- SSE-ImGui project for D3D11 hooking reference
- OpenDyslexic font
- mjorka for Sovngarde font
- Community Shaders team (input reference)
- Paul Renner for Futura font
