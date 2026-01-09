#include "Settings.h"
#include "PCH.h"

namespace Easy2Read {

Settings *Settings::GetSingleton() {
  static Settings singleton;
  return &singleton;
}

void Settings::Load() {
  constexpr auto path = L"Data/SKSE/Plugins/Easy2Read.ini";

  CSimpleIniA ini;
  ini.SetUnicode();

  const auto rc = ini.LoadFile(path);
  if (rc < 0) {
    SKSE::log::warn("Easy2Read.ini not found, using defaults");
  } else {
    SKSE::log::info("Loading configuration from Easy2Read.ini");

    // [General] - only hotkey in main config
    toggleKey = static_cast<std::uint32_t>(
        ini.GetLongValue("General", "ToggleKey", 33));
    SKSE::log::info("  ToggleKey: {}", toggleKey);
  }

  // Load theming from separate file
  LoadTheme();
}

void Settings::LoadTheme() {
  constexpr auto themePath = L"Data/SKSE/Plugins/Easy2Read_Theme.ini";

  CSimpleIniA ini;
  ini.SetUnicode();

  const auto rc = ini.LoadFile(themePath);
  if (rc < 0) {
    SKSE::log::warn("Easy2Read_Theme.ini not found, using default theme");
    return;
  }

  SKSE::log::info("Loading theme from Easy2Read_Theme.ini");

  // [Font]
  const char *fontPresetStr = ini.GetValue("Font", "FontPreset", "Sovngarde");
  fontPreset = ParseFontPreset(fontPresetStr);
  customFontFile = ini.GetValue("Font", "CustomFontFile",
                                "SKSE/Plugins/Easy2Read/CustomFont.ttf");
  fontSize = static_cast<float>(ini.GetDoubleValue("Font", "FontSize", 24.0));
  titleScale =
      static_cast<float>(ini.GetDoubleValue("Font", "TitleScale", 1.2));

  // [Colors] - Title
  titleColorR =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "TitleColorR", 255));
  titleColorG =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "TitleColorG", 220));
  titleColorB =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "TitleColorB", 150));

  // [Colors] - Body
  bodyColorR =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "BodyColorR", 255));
  bodyColorG =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "BodyColorG", 255));
  bodyColorB =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "BodyColorB", 255));

  // [Colors] - Window
  windowColorR =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "WindowColorR", 20));
  windowColorG =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "WindowColorG", 20));
  windowColorB =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "WindowColorB", 25));

  // [Colors] - Border
  borderColorR =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "BorderColorR", 80));
  borderColorG =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "BorderColorG", 80));
  borderColorB =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "BorderColorB", 90));
  borderSize =
      static_cast<float>(ini.GetDoubleValue("Colors", "BorderSize", 1.0));

  // [Colors] - Separator
  separatorColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "SeparatorColorR", 100));
  separatorColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "SeparatorColorG", 100));
  separatorColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "SeparatorColorB", 110));

  // [Scrollbar] - Background
  scrollbarBgColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "BackgroundColorR", 30));
  scrollbarBgColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "BackgroundColorG", 30));
  scrollbarBgColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "BackgroundColorB", 35));

  // [Scrollbar] - Thumb
  scrollbarColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbColorR", 80));
  scrollbarColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbColorG", 80));
  scrollbarColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbColorB", 90));

  // [Scrollbar] - Thumb Hover
  scrollbarHoverColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbHoverColorR", 120));
  scrollbarHoverColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbHoverColorG", 120));
  scrollbarHoverColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbHoverColorB", 130));

  // [Scrollbar] - Size/Speed
  scrollbarSize =
      static_cast<float>(ini.GetDoubleValue("Scrollbar", "Size", 14.0));
  scrollbarRounding =
      static_cast<float>(ini.GetDoubleValue("Scrollbar", "Rounding", 4.0));
  scrollSpeed =
      static_cast<float>(ini.GetDoubleValue("Scrollbar", "ScrollSpeed", 50.0));

  // [Window]
  windowWidthPercent =
      static_cast<float>(ini.GetDoubleValue("Window", "WidthPercent", 50.0));
  windowHeightPercent =
      static_cast<float>(ini.GetDoubleValue("Window", "HeightPercent", 70.0));
  windowOpacity =
      static_cast<float>(ini.GetLongValue("Window", "Opacity", 90)) / 100.0f;
  windowRounding =
      static_cast<float>(ini.GetDoubleValue("Window", "Rounding", 8.0));
  windowPadding =
      static_cast<float>(ini.GetDoubleValue("Window", "Padding", 12.0));

  SKSE::log::info("  FontPreset: {}, FontSize: {}, TitleScale: {}",
                  fontPresetStr, fontSize, titleScale);
  SKSE::log::info("  WindowSize: {}%x{}%, Opacity: {:.0f}%", windowWidthPercent,
                  windowHeightPercent, windowOpacity * 100);
}

std::string Settings::GetFontPath() const {
  switch (fontPreset) {
  case FontPreset::Sovngarde:
    return "Data/SKSE/Plugins/Easy2Read/Sovngarde-Bold.ttf";
  case FontPreset::Dyslexic:
    return "Data/SKSE/Plugins/Easy2Read/OpenDyslexic-Regular.otf";
  case FontPreset::Custom:
    return "Data/" + customFontFile;
  case FontPreset::ImGuiDefault:
  default:
    return ""; // Empty means use ImGui default font
  }
}

FontPreset Settings::ParseFontPreset(const std::string &str) {
  if (str == "sovngarde" || str == "Sovngarde" || str == "SOVNGARDE") {
    return FontPreset::Sovngarde;
  } else if (str == "dyslexic" || str == "Dyslexic" || str == "DYSLEXIC") {
    return FontPreset::Dyslexic;
  } else if (str == "custom" || str == "Custom" || str == "CUSTOM") {
    return FontPreset::Custom;
  } else if (str == "imgui" || str == "ImGui" || str == "IMGUI" ||
             str == "default" || str == "Default" || str == "DEFAULT") {
    return FontPreset::ImGuiDefault;
  }
  return FontPreset::Sovngarde; // Default to Sovngarde
}

} // namespace Easy2Read
