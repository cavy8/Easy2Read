#pragma once

#include <cstdint>
#include <string>

namespace Easy2Read {

enum class FontPreset {
  Sovngarde,    // Sovngarde font - Skyrim-themed (new default)
  Dyslexic,     // OpenDyslexic - accessibility font
  ImGuiDefault, // Built-in ImGui font
  Custom        // User-specified font file
};

class Settings {
public:
  [[nodiscard]] static Settings *GetSingleton();

  void Load();
  void LoadTheme();

  // Get the resolved font file path based on current preset
  [[nodiscard]] std::string GetFontPath() const;

  // Convert string to FontPreset enum
  static FontPreset ParseFontPreset(const std::string &str);

  // ---- Overlay ----
  bool overlayEnabled = true;   // Set to false to disable overlay entirely
  std::uint32_t toggleKey = 33; // F key (0x21)
  std::uint32_t controllerToggleButton =
      0x8000;                         // Y button on Xbox (0x8000 = 32768)
  float controllerScrollSpeed = 3.0f; // Scroll speed for controller thumbstick

  // ---- Font ----
  FontPreset fontPreset = FontPreset::Sovngarde;
  std::string customFontFile;
  float fontSize = 24.0f;      // Body text size
  float titleFontSize = 28.0f; // Title text size (0 = same as body)
  float titleScale =
      1.2f; // Scale multiplier for title (alternative to titleFontSize)

  // ---- Window (percentage of screen size, 0-100) ----
  float windowWidthPercent = 50.0f;  // 50% of screen width
  float windowHeightPercent = 70.0f; // 70% of screen height
  float windowRounding = 8.0f;       // Corner rounding in pixels
  float windowPadding = 12.0f;       // Padding inside window

  // ---- Colors (RGB, 0-255) ----
  // Title text
  std::uint8_t titleColorR = 255;
  std::uint8_t titleColorG = 220;
  std::uint8_t titleColorB = 150;

  // Body text
  std::uint8_t bodyColorR = 255;
  std::uint8_t bodyColorG = 255;
  std::uint8_t bodyColorB = 255;

  // Window background
  std::uint8_t windowColorR = 20;
  std::uint8_t windowColorG = 20;
  std::uint8_t windowColorB = 25;

  // Window border
  std::uint8_t borderColorR = 80;
  std::uint8_t borderColorG = 80;
  std::uint8_t borderColorB = 90;
  float borderSize = 1.0f;

  // Separator line
  std::uint8_t separatorColorR = 100;
  std::uint8_t separatorColorG = 100;
  std::uint8_t separatorColorB = 110;

  // Scrollbar background
  std::uint8_t scrollbarBgColorR = 30;
  std::uint8_t scrollbarBgColorG = 30;
  std::uint8_t scrollbarBgColorB = 35;

  // Scrollbar thumb (handle)
  std::uint8_t scrollbarColorR = 80;
  std::uint8_t scrollbarColorG = 80;
  std::uint8_t scrollbarColorB = 90;

  // Scrollbar thumb hover
  std::uint8_t scrollbarHoverColorR = 120;
  std::uint8_t scrollbarHoverColorG = 120;
  std::uint8_t scrollbarHoverColorB = 130;

  // Scrollbar settings
  float scrollbarSize = 14.0f;    // Width of scrollbar
  float scrollbarRounding = 4.0f; // Rounding of scrollbar corners
  float scrollSpeed = 50.0f;      // Pixels per scroll wheel tick

  // ---- Visibility Toggles ----
  bool showTitle = true;          // Show book title
  bool showSeparator = true;      // Show separator line under title
  bool showBorder = true;         // Show window border
  bool showScrollbarTrack = true; // Show scrollbar background track

  // ---- Per-Element Transparency (0-1) ----
  float windowAlpha = 0.90f;        // Window background transparency
  float borderAlpha = 1.0f;         // Border transparency
  float separatorAlpha = 1.0f;      // Separator line transparency
  float scrollbarTrackAlpha = 1.0f; // Scrollbar background transparency
  float scrollbarThumbAlpha = 1.0f; // Scrollbar thumb transparency

  // ---- Text Sanitization ----
  bool sanitizationEnabled = true;
  std::string sanitizationMode = "On"; // "Off", "DetectOnly", "On"
  bool sanitizationDebugMode = false;
  bool sanitizationLogReplacements = false;
  float sanitizationMaxExpansionRatio = 3.0f;

  // ---- Per-hook enable settings ----
  bool hookEnableDescription = true;
  bool hookEnableDialogue = true;
  bool hookEnableDialogueMenu = true;
  bool hookEnableQuest = true;
  bool hookEnableMapMarker = true;
  bool hookEnableNpcName = true;

private:
  Settings() = default;
  Settings(const Settings &) = delete;
  Settings(Settings &&) = delete;
  ~Settings() = default;

  Settings &operator=(const Settings &) = delete;
  Settings &operator=(Settings &&) = delete;
};

} // namespace Easy2Read
