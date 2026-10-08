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

    // [General] - overlay and hotkey settings
    overlayEnabled = ini.GetBoolValue("General", "EnableOverlay", true);
    toggleKey = static_cast<std::uint32_t>(
        ini.GetLongValue("General", "ToggleKey", 33));
    controllerToggleButton = static_cast<std::uint32_t>(
        ini.GetLongValue("General", "ControllerToggleButton", 0x8000));
    controllerScrollSpeed = static_cast<float>(
        ini.GetDoubleValue("General", "ControllerScrollSpeed", 3.0));
    SKSE::log::info(
        "  Overlay: {} (key: {}, controller: 0x{:X}, scroll speed: {})",
        overlayEnabled ? "enabled" : "disabled", toggleKey,
        controllerToggleButton, controllerScrollSpeed);
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

  const char *langSupportStr =
      ini.GetValue("Font", "LanguageSupport", "European");
  languageSupport = ParseLanguageSupport(langSupportStr);

  // [Colors] - Title
  titleColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "TitleColorR", 255));
  titleColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "TitleColorG", titleColorG));
  titleColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "TitleColorB", titleColorB));

  // [Colors] - Body
  bodyColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "BodyColorR", 255));
  bodyColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "BodyColorG", 255));
  bodyColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "BodyColorB", 255));

  // [Colors] - Window
  windowColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "WindowColorR", windowColorR));
  windowColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "WindowColorG", windowColorG));
  windowColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "WindowColorB", windowColorB));

  // [Colors] - Border
  borderColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "BorderColorR", borderColorR));
  borderColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "BorderColorG", borderColorG));
  borderColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "BorderColorB", borderColorB));
  borderSize =
      static_cast<float>(ini.GetDoubleValue("Colors", "BorderSize", 1.0));

  // [Colors] - Separator
  separatorColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "SeparatorColorR", separatorColorR));
  separatorColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "SeparatorColorG", separatorColorG));
  separatorColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Colors", "SeparatorColorB", separatorColorB));

  // [Scrollbar] - Background
  scrollbarBgColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "BackgroundColorR", scrollbarBgColorR));
  scrollbarBgColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "BackgroundColorG", scrollbarBgColorG));
  scrollbarBgColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "BackgroundColorB", scrollbarBgColorB));

  // [Scrollbar] - Thumb
  scrollbarColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbColorR", scrollbarColorR));
  scrollbarColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbColorG", scrollbarColorG));
  scrollbarColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbColorB", scrollbarColorB));

  // [Scrollbar] - Thumb Hover
  scrollbarHoverColorR = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbHoverColorR", scrollbarHoverColorR));
  scrollbarHoverColorG = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbHoverColorG", scrollbarHoverColorG));
  scrollbarHoverColorB = static_cast<std::uint8_t>(
      ini.GetLongValue("Scrollbar", "ThumbHoverColorB", scrollbarHoverColorB));

  // [Scrollbar] - Size/Speed
  scrollbarSize =
      static_cast<float>(ini.GetDoubleValue("Scrollbar", "Size", scrollbarSize));
  scrollbarRounding = static_cast<float>(
      ini.GetDoubleValue("Scrollbar", "Rounding", scrollbarRounding));
  scrollSpeed =
      static_cast<float>(ini.GetDoubleValue("Scrollbar", "ScrollSpeed", 50.0));

  // [Window]
  windowWidthPercent =
      static_cast<float>(ini.GetDoubleValue("Window", "WidthPercent", 50.0));
  windowHeightPercent =
      static_cast<float>(ini.GetDoubleValue("Window", "HeightPercent", 70.0));
  windowRounding =
      static_cast<float>(ini.GetDoubleValue("Window", "Rounding", windowRounding));
  windowPadding =
      static_cast<float>(ini.GetDoubleValue("Window", "Padding", windowPadding));

  // [Visibility]
  // Older theme files retain their original title layout and plain border.
  centerTitle = ini.GetBoolValue("Visibility", "CenterTitle", false);
  showCornerOrnaments =
      ini.GetBoolValue("Visibility", "ShowCornerOrnaments", false);
  showTitle = ini.GetBoolValue("Visibility", "ShowTitle", true);
  showSeparator = ini.GetBoolValue("Visibility", "ShowSeparator", true);
  showBorder = ini.GetBoolValue("Visibility", "ShowBorder", true);
  showScrollbarTrack =
      ini.GetBoolValue("Visibility", "ShowScrollbarTrack", true);

  // [Transparency]
  windowAlpha =
      static_cast<float>(ini.GetLongValue("Transparency", "WindowAlpha", 80)) /
      100.0f;
  borderAlpha =
      static_cast<float>(ini.GetLongValue("Transparency", "BorderAlpha", 85)) /
      100.0f;
  separatorAlpha = static_cast<float>(ini.GetLongValue("Transparency",
                                                       "SeparatorAlpha", 75)) /
                   100.0f;
  scrollbarTrackAlpha = static_cast<float>(ini.GetLongValue(
                            "Transparency", "ScrollbarTrackAlpha", 50)) /
                        100.0f;
  scrollbarThumbAlpha = static_cast<float>(ini.GetLongValue(
                            "Transparency", "ScrollbarThumbAlpha", 100)) /
                        100.0f;

  SKSE::log::info(
      "  FontPreset: {}, FontSize: {}, TitleScale: {}, LanguageSupport: {}",
      fontPresetStr, fontSize, titleScale,
      ini.GetValue("Font", "LanguageSupport", "European"));
  SKSE::log::info("  WindowSize: {}%x{}%, WindowAlpha: {:.0f}%",
                  windowWidthPercent, windowHeightPercent, windowAlpha * 100);
  SKSE::log::info(
      "  Visibility: Title={}, Separator={}, Border={}, ScrollbarTrack={}",
      showTitle, showSeparator, showBorder, showScrollbarTrack);
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

LanguageSupport Settings::ParseLanguageSupport(const std::string &str) {
  if (str == "latin" || str == "Latin" || str == "LATIN") {
    return LanguageSupport::Latin;
  } else if (str == "european" || str == "European" || str == "EUROPEAN") {
    return LanguageSupport::European;
  } else if (str == "asian" || str == "Asian" || str == "ASIAN" ||
             str == "cjk" || str == "CJK") {
    return LanguageSupport::Asian;
  } else if (str == "full" || str == "Full" || str == "FULL" || str == "all" ||
             str == "All" || str == "ALL") {
    return LanguageSupport::Full;
  }
  return LanguageSupport::European; // Default to European
}

} // namespace Easy2Read
