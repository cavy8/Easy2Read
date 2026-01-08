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
    return;
  }

  SKSE::log::info("Loading configuration from Easy2Read.ini");

  // [General]
  toggleKey =
      static_cast<std::uint32_t>(ini.GetLongValue("General", "ToggleKey", 33));

  // [Font]
  const char *fontPresetStr = ini.GetValue("Font", "Font", "default");
  fontPreset = ParseFontPreset(fontPresetStr);

  customFontFile = ini.GetValue("Font", "CustomFontFile", "");
  fontSize = static_cast<float>(ini.GetDoubleValue("Font", "FontSize", 18.0));

  // [Window]
  windowWidth =
      static_cast<float>(ini.GetDoubleValue("Window", "Width", 600.0));
  windowHeight =
      static_cast<float>(ini.GetDoubleValue("Window", "Height", 400.0));
  backgroundAlpha =
      static_cast<float>(ini.GetDoubleValue("Window", "BackgroundAlpha", 0.95));
  closeOnClickOutside =
      ini.GetBoolValue("Window", "CloseOnClickOutside", false);

  // Parse hex colors
  const char *textColorStr = ini.GetValue("Window", "TextColor", "E0E0E0FF");
  const char *bgColorStr =
      ini.GetValue("Window", "BackgroundColor", "1A1A1AFF");

  textColor = static_cast<std::uint32_t>(std::stoul(textColorStr, nullptr, 16));
  backgroundColor =
      static_cast<std::uint32_t>(std::stoul(bgColorStr, nullptr, 16));

  SKSE::log::info("  ToggleKey: {}", toggleKey);
  SKSE::log::info("  FontPreset: {}", fontPresetStr);
  SKSE::log::info("  FontSize: {}", fontSize);
  SKSE::log::info("  WindowSize: {}x{}", windowWidth, windowHeight);
  SKSE::log::info("  CloseOnClickOutside: {}", closeOnClickOutside);
}

std::string Settings::GetFontPath() const {
  switch (fontPreset) {
  case FontPreset::Default:
    return "Data/SKSE/Plugins/Easy2Read/Roboto-Regular.ttf";
  case FontPreset::Dyslexic:
    return "Data/SKSE/Plugins/Easy2Read/OpenDyslexic-Regular.otf";
  case FontPreset::Custom:
    return customFontFile;
  default:
    return "Data/SKSE/Plugins/Easy2Read/Roboto-Regular.ttf";
  }
}

FontPreset Settings::ParseFontPreset(const std::string &str) {
  if (str == "dyslexic" || str == "Dyslexic" || str == "DYSLEXIC") {
    return FontPreset::Dyslexic;
  } else if (str == "custom" || str == "Custom" || str == "CUSTOM") {
    return FontPreset::Custom;
  }
  return FontPreset::Default; // "default" or any other value
}

} // namespace Easy2Read
