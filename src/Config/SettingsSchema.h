#pragma once

#include "Settings.h"
#include <variant>
#include <string_view>

namespace Easy2Read {
// Shared by the framework editor and INI writer to keep their coverage identical.
using SettingMember = std::variant<bool Settings::*, float Settings::*,
    std::uint32_t Settings::*, std::uint8_t Settings::*, std::string Settings::*,
    FontPreset Settings::*, LanguageSupport Settings::*>;
struct SettingField {
  const char *section;
  const char *key;
  const char *label;
  SettingMember member;
  float minimum;
  float maximum;
  bool refreshResources;
  bool IsGeneral() const { return std::string_view(section) == "General"; }
};
inline const SettingField settingFields[] = {
    {"General", "EnableOverlay", "Enable reading overlay", &Settings::overlayEnabled, 0.0f, 255.0f, true},
    {"General", "ShowBookPrompt", "Show book-screen prompt", &Settings::showBookPrompt, 0.0f, 255.0f, true},
    {"General", "ToggleKey", "Toggle Key", &Settings::toggleKey, 1.0f, 255.0f, true},
    {"General", "ControllerToggleButton", "Controller Toggle Button", &Settings::controllerToggleButton, 1.0f, 65535.0f, true},
    {"General", "ControllerScrollSpeed", "Controller Scroll Speed", &Settings::controllerScrollSpeed, 0.0f, 100.0f, false},
    {"Font", "FontPreset", "Typeface", &Settings::fontPreset, 0.0f, 255.0f, true},
    {"Font", "CustomFontFile", "Custom Font File", &Settings::customFontFile, 0.0f, 255.0f, true},
    {"Font", "FontSize", "Body text size", &Settings::fontSize, 8.0f, 96.0f, true},
    {"Font", "TitleScale", "Title text size (relative to body)", &Settings::titleScale, 0.5f, 4.0f, false},
    {"Font", "LanguageSupport", "Character coverage", &Settings::languageSupport, 0.0f, 255.0f, true},
    {"Colors", "TitleColorR", "Title Color R", &Settings::titleColorR, 0.0f, 255.0f, false},
    {"Colors", "TitleColorG", "Title Color G", &Settings::titleColorG, 0.0f, 255.0f, false},
    {"Colors", "TitleColorB", "Title Color B", &Settings::titleColorB, 0.0f, 255.0f, false},
    {"Colors", "BodyColorR", "Body Color R", &Settings::bodyColorR, 0.0f, 255.0f, false},
    {"Colors", "BodyColorG", "Body Color G", &Settings::bodyColorG, 0.0f, 255.0f, false},
    {"Colors", "BodyColorB", "Body Color B", &Settings::bodyColorB, 0.0f, 255.0f, false},
    {"Colors", "WindowColorR", "Window Color R", &Settings::windowColorR, 0.0f, 255.0f, false},
    {"Colors", "WindowColorG", "Window Color G", &Settings::windowColorG, 0.0f, 255.0f, false},
    {"Colors", "WindowColorB", "Window Color B", &Settings::windowColorB, 0.0f, 255.0f, false},
    {"Colors", "BorderColorR", "Border Color R", &Settings::borderColorR, 0.0f, 255.0f, false},
    {"Colors", "BorderColorG", "Border Color G", &Settings::borderColorG, 0.0f, 255.0f, false},
    {"Colors", "BorderColorB", "Border Color B", &Settings::borderColorB, 0.0f, 255.0f, false},
    {"Colors", "BorderSize", "Border / artwork scale", &Settings::borderSize, 0.0f, 10.0f, false},
    {"Colors", "SeparatorColorR", "Separator Color R", &Settings::separatorColorR, 0.0f, 255.0f, false},
    {"Colors", "SeparatorColorG", "Separator Color G", &Settings::separatorColorG, 0.0f, 255.0f, false},
    {"Colors", "SeparatorColorB", "Separator Color B", &Settings::separatorColorB, 0.0f, 255.0f, false},
    {"Scrollbar", "BackgroundColorR", "Background Color R", &Settings::scrollbarBgColorR, 0.0f, 255.0f, false},
    {"Scrollbar", "BackgroundColorG", "Background Color G", &Settings::scrollbarBgColorG, 0.0f, 255.0f, false},
    {"Scrollbar", "BackgroundColorB", "Background Color B", &Settings::scrollbarBgColorB, 0.0f, 255.0f, false},
    {"Scrollbar", "ThumbColorR", "Thumb Color R", &Settings::scrollbarColorR, 0.0f, 255.0f, false},
    {"Scrollbar", "ThumbColorG", "Thumb Color G", &Settings::scrollbarColorG, 0.0f, 255.0f, false},
    {"Scrollbar", "ThumbColorB", "Thumb Color B", &Settings::scrollbarColorB, 0.0f, 255.0f, false},
    {"Scrollbar", "ThumbHoverColorR", "Thumb Hover Color R", &Settings::scrollbarHoverColorR, 0.0f, 255.0f, false},
    {"Scrollbar", "ThumbHoverColorG", "Thumb Hover Color G", &Settings::scrollbarHoverColorG, 0.0f, 255.0f, false},
    {"Scrollbar", "ThumbHoverColorB", "Thumb Hover Color B", &Settings::scrollbarHoverColorB, 0.0f, 255.0f, false},
    {"Scrollbar", "Size", "Size", &Settings::scrollbarSize, 1.0f, 50.0f, false},
    {"Scrollbar", "Rounding", "Rounding", &Settings::scrollbarRounding, 0.0f, 50.0f, false},
    {"Scrollbar", "ScrollSpeed", "Mouse wheel scroll speed", &Settings::scrollSpeed, 1.0f, 500.0f, false},
    {"Window", "WidthPercent", "Window width", &Settings::windowWidthPercent, 10.0f, 100.0f, false},
    {"Window", "HeightPercent", "Window height", &Settings::windowHeightPercent, 10.0f, 100.0f, false},
    {"Window", "Rounding", "Rounding", &Settings::windowRounding, 0.0f, 100.0f, false},
    {"Window", "Padding", "Padding", &Settings::windowPadding, 0.0f, 100.0f, false},
    {"Visibility", "CenterTitle", "Center Title", &Settings::centerTitle, 0.0f, 255.0f, false},
    {"Visibility", "ShowCornerOrnaments", "Show Corner Ornaments", &Settings::showCornerOrnaments, 0.0f, 255.0f, false},
    {"Visibility", "ShowTitle", "Show Title", &Settings::showTitle, 0.0f, 255.0f, false},
    {"Visibility", "ShowSeparator", "Show Separator", &Settings::showSeparator, 0.0f, 255.0f, false},
    {"Visibility", "ShowBorder", "Show Border", &Settings::showBorder, 0.0f, 255.0f, false},
    {"Visibility", "ShowScrollbarTrack", "Show Scrollbar Track", &Settings::showScrollbarTrack, 0.0f, 255.0f, false},
    {"Transparency", "WindowAlpha", "Background opacity", &Settings::windowAlpha, 0.0f, 1.0f, false},
    {"Transparency", "BorderAlpha", "Border opacity", &Settings::borderAlpha, 0.0f, 1.0f, false},
    {"Transparency", "SeparatorAlpha", "Separator opacity", &Settings::separatorAlpha, 0.0f, 1.0f, false},
    {"Transparency", "ScrollbarTrackAlpha", "Track opacity", &Settings::scrollbarTrackAlpha, 0.0f, 1.0f, false},
    {"Transparency", "ScrollbarThumbAlpha", "Thumb opacity", &Settings::scrollbarThumbAlpha, 0.0f, 1.0f, false},
};
inline constexpr const char *fontPresetNames[] = {"Barlow", "Sovngarde", "Dyslexic", "ImGui", "Custom"};
inline constexpr const char *languageSupportNames[] = {"Latin", "European", "Asian", "Full"};
struct ColorSetting {
  const char *label;
  std::uint8_t Settings::* channels[3];
};
inline constexpr ColorSetting colorSettings[] = {
    {"Title text", {&Settings::titleColorR, &Settings::titleColorG, &Settings::titleColorB}},
    {"Body text", {&Settings::bodyColorR, &Settings::bodyColorG, &Settings::bodyColorB}},
    {"Window background", {&Settings::windowColorR, &Settings::windowColorG, &Settings::windowColorB}},
    {"Border", {&Settings::borderColorR, &Settings::borderColorG, &Settings::borderColorB}},
    {"Separator", {&Settings::separatorColorR, &Settings::separatorColorG, &Settings::separatorColorB}},
    {"Track", {&Settings::scrollbarBgColorR, &Settings::scrollbarBgColorG, &Settings::scrollbarBgColorB}},
    {"Thumb", {&Settings::scrollbarColorR, &Settings::scrollbarColorG, &Settings::scrollbarColorB}},
    {"Thumb when hovered", {&Settings::scrollbarHoverColorR, &Settings::scrollbarHoverColorG, &Settings::scrollbarHoverColorB}},
};
} // namespace Easy2Read
