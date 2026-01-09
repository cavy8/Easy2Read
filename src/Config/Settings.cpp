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

  // [Colors]
  titleColorR =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "TitleColorR", 255));
  titleColorG =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "TitleColorG", 220));
  titleColorB =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "TitleColorB", 150));

  bodyColorR =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "BodyColorR", 255));
  bodyColorG =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "BodyColorG", 255));
  bodyColorB =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "BodyColorB", 255));

  windowColorR =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "WindowColorR", 20));
  windowColorG =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "WindowColorG", 20));
  windowColorB =
      static_cast<std::uint8_t>(ini.GetLongValue("Colors", "WindowColorB", 25));

  // [Window] - now percentage-based
  windowWidthPercent =
      static_cast<float>(ini.GetDoubleValue("Window", "WidthPercent", 50.0));
  windowHeightPercent =
      static_cast<float>(ini.GetDoubleValue("Window", "HeightPercent", 70.0));
  windowOpacity =
      static_cast<float>(ini.GetLongValue("Window", "Opacity", 90)) / 100.0f;

  SKSE::log::info("  FontPreset: {}, FontSize: {}", fontPresetStr, fontSize);
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
