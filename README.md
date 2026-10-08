# Easy2Read

An SKSE plugin for Skyrim SE/AE that displays book and note text in a custom overlay window with configurable fonts for improved readability.

## Features

When reading a book/note, press the F key (configurable) to pull up an overlay with the text. Theme support, multiple font options (including the OpenDyslexic font), and includes support for image descriptions/text replacement. By default, mappings are provided for vanilla and Scribes of Skyrim calligraphy.

## Installation

1. Install [SKSE](https://skse.silverlock.org/) for your Skyrim version
2. Install [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444) for your game version (SE for 1.5.x; AE for 1.6.x/1.7.x).
3. Install `Easy2Read.dll` in `Data/SKSE/Plugins/` and copy the supplied `Data` contents into your game's `Data` directory.
4. Install [ImGui Icons](https://www.nexusmods.com/skyrimspecialedition/mods/114790)
   for the book-screen button artwork. Its replacement PNG style patches are
   supported. Missing artwork falls back to a text badge.

### Runtime compatibility

One DLL targets Skyrim SE 1.5.97 and AE 1.6.x/1.7.x, including 1.6.1130,
1.6.1170, GOG 1.6.1179, and 1.7.104. Each runtime needs its matching SKSE and
Address Library.

## Usage

### Book Overlay
1. Open any book or note in Skyrim
   A centered button icon followed by **Show Text** appears below the model,
   matching the configured keyboard key or Xbox controller button.
2. Press the toggle key (default: **F**) to display the overlay
3. Scroll with your mouse wheel to read long texts
4. Press the toggle key again or close the book to hide the overlay

The prompt hides while the text overlay is open. It always stays horizontally centered
on the screen; its height follows the model's projected bottom edge and stays above
the bottom control strip. If bounds are unavailable,
it falls back to the lower center of the screen. Very large models may overlap it
when there is no room underneath. Set `ShowBookPrompt = false` to hide the prompt.

## Configuration

### Optional in-game settings

Install [SKSE Menu Framework 3](https://github.com/QTR-Modding/SKSE-Menu-Framework-3)
and its requirements to add **Easy2Read > General** and **Easy2Read > Theme**
to its Mod Control Panel. Easy2Read also works without the framework.

Cleared bindings use `0` in the INI and hide the prompt for that device. Bindings
the framework cannot represent remain intact until reassigned or explicitly cleared.

### Easy2Read.ini

```ini
[General]
ToggleKey = 33  ; Hotkey scancode (default F = 33)
ShowBookPrompt = true  ; Show the key/button hint on the book screen
```

### Easy2Read_Theme.ini

The default theme follows SkyUI/Vanilla menus: translucent black, white text,
centered titles, the original SkyUI message-box frame and corner artwork, and
square scrollbars. It uses bundled Barlow Condensed Regular at 24 px for a similar
look to Skyrim's Futura CondensedLight menu font.

The previous default is now **Modern**. To install it, copy the contents of `Presets/Modern`
into your game's `Data` directory and overwrite `Easy2Read_Theme.ini`. Install
`Presets/Untarnished UI` the same way. Reinstall the supplied `Data` theme file to
restore the new default, then restart Skyrim to load the theme.

Customize the overlay appearance:

- **[Font]**: FontPreset (Barlow/Sovngarde/Dyslexic/ImGui/Custom), FontSize, TitleScale
- **[Colors]**: Title, body text, window, border, separator colors (RGB 0-255)
- **[Scrollbar]**: Background, thumb, hover colors, size, rounding, scroll speed
- **[Window]**: Size (% of screen), rounding, padding
- **[Visibility]**: ShowTitle, CenterTitle, ShowSeparator, ShowBorder, ShowCornerOrnaments, ShowScrollbarTrack
- **[Transparency]**: WindowAlpha, BorderAlpha, SeparatorAlpha, ScrollbarTrackAlpha, ScrollbarThumbAlpha (0-100)

`ShowCornerOrnaments` requires `ShowBorder = true` and a positive `BorderSize`.
It replaces the plain border with SkyUI's extracted frame; `BorderSize = 1.0`
preserves the artwork's native proportions at 1080p, and other values scale it.
The corners scale uniformly with screen height while the middle strips stretch
to fit the window. The artwork is embedded in the DLL.
Existing theme files that omit `CenterTitle` and `ShowCornerOrnaments` retain
left-aligned titles and plain borders.

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

The optional settings check runs without Skyrim in an isolated directory:

```powershell
cmake --build build --config Release --target SettingsPersistence
build/Release/SettingsPersistence.exe . build/settings-test-data
```

It checks shipped INI/preset coverage, save/reload isolation, comment preservation,
save failures, binding conversions, and color picker coverage. In-game checks:
open both Easy2Read pages, assign keyboard/controller bindings, change fonts and
colors with a book open, save/reload each file, restart to check persistence, and
also launch without SKSE Menu Framework.

### Build release archives

From the repository folder, run:

```powershell
.\build-release.cmd
# Add a test label and a separate debug-symbol archive:
.\build-release.cmd -Suffix test -IncludeSymbols
```

The launcher runs `build-release.ps1` with a process-only execution-policy bypass,
so you don't need to change your Windows script policy. You can also run the
PowerShell script directly if your execution policy allows it.

The script configures and builds Release in `build/release-package` using the
requirements above. It reads the version from `CMakeLists.txt` and writes to `dist`:

- `Easy2Read-<version>.zip`: DLL, configs, bundled fonts, image mappings, licenses, and documentation. Install directly with your mod manager, or extract into Skyrim's `Data` folder.
- `Easy2Read-<version>-Theme-Modern.zip` and `Easy2Read-<version>-Theme-Untarnished-UI.zip`: optional theme overrides. Install one after the main package and let it overwrite the theme file.
- `Easy2Read-<version>-Symbols.zip`: PDB for debugging, only with `-IncludeSymbols`.

`-BuildDirectory` and `-OutputDirectory` accept paths relative to the repository
or absolute paths. The script stops on build failures, prints each archive's
SHA256, and temporarily disables `SKYRIM_MODS_FOLDER` so packaging does not install
into your game. Re-running replaces archives with the same version and suffix.

## Dependencies

- [CommonLibSSE NG](https://github.com/alandtse/CommonLibSSE-NG/tree/ng) - SKSE plugin framework
- [ImGui](https://github.com/ocornut/imgui) - Overlay rendering
- [ImGui Icons](https://www.nexusmods.com/skyrimspecialedition/mods/114790) - Button artwork, loaded from the installed mod
- [DirectX Tool Kit](https://github.com/microsoft/DirectXTK) - PNG texture loading
- [SimpleIni](https://github.com/brofield/simpleini) - INI file parsing
- [SKSE Menu Framework 3](https://github.com/QTR-Modding/SKSE-Menu-Framework-3) - Optional in-game settings; API source and license details are in `src/ThirdParty/README.md`

## Future Plans

- Better foreign language support

## License

This project is licensed under GPL-3.0. Barlow Condensed Light, Sovngarde, and OpenDyslexic font files are licensed under the SIL Open Font License; their notices are included in `Data/SKSE/Plugins/Easy2Read/Licenses`. Futura font file is from dafontfamily.com, license not specified.

I can't stop you from doing anything you want with this. That said, I'd still appreciate it if you reached out to me first :)

## Credits

- CommonLibSSE NG team for the SKSE framework
- ImGui for the immediate-mode GUI library
- powerofthree for ImGui Icons
- SSE-ImGui project for D3D11 hooking reference
- OpenDyslexic font
- mjorka for Sovngarde font
- Jeremy Tribby and the Barlow Project Authors for Barlow Condensed Regular
- Community Shaders team (input reference)
- SkyUI team for the message-box frame artwork, extracted from [SkyUI Community](https://github.com/doodlum/SkyUI-Community). See [extraction details](assets/SkyUI/README.md).
- Paul Renner for Futura font
