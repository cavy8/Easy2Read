# Easy2Read

An SKSE plugin for Skyrim SE/AE that displays book and note text in a custom overlay window with configurable fonts for improved readability.

## Features

- **Customizable Overlay**: Display book text in a clean, readable overlay
- **Font Options**: Choose from Sovngarde (Skyrim-themed), OpenDyslexic (accessibility), or custom fonts
- **Full Theming**: Configure colors, opacity, scrollbar, borders, and more
- **Scroll Wheel Support**: Scroll through long texts with your mouse wheel
- **Hotkey Toggle**: Press a configurable key to show/hide the overlay while reading

## Installation

1. Install [SKSE](https://skse.silverlock.org/) for your Skyrim version
2. Copy `Easy2Read.dll` to `Data/SKSE/Plugins/`
3. Copy `Easy2Read.ini` and `Easy2Read_Theme.ini` to `Data/SKSE/Plugins/`
4. (Optional) Copy font files to `Data/SKSE/Plugins/Easy2Read/`

## Usage

1. Open any book or note in Skyrim
2. Press the toggle key (default: **F**) to display the overlay
3. Scroll with your mouse wheel to read long texts
4. Press the toggle key again or close the book to hide the overlay

## Configuration

### Easy2Read.ini

```ini
[General]
; Hotkey scancode (default F = 33)
ToggleKey = 33
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

1. **VR Not Supported**: This implementation targets SE/AE only
2. **Multi-page Books**: All text is shown concatenated; original page breaks are not preserved
3. **Dynamic Text**: Books with script-generated text may show base text
4. **Input Capturing**: Scroll wheel input is still passed onto the underlying book menu.

## License

This project is open source. Font files are licensed under the SIL Open Font License.

## Credits

- CommonLibSSE NG team for the SKSE framework
- ImGui for the immediate-mode GUI library
- SSE-ImGui project for D3D11 hooking reference
