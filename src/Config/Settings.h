#pragma once

#include <cstdint>
#include <string>

namespace Easy2Read {

enum class FontPreset {
  Barlow,       // Barlow Condensed Regular - vanilla menu-inspired (default)
  Sovngarde,    // Sovngarde font - Skyrim-themed
  Dyslexic,     // OpenDyslexic - accessibility font
  ImGuiDefault, // Built-in ImGui font
  Custom        // User-specified font file
};

enum class LanguageSupport {
  Latin,    // Basic Latin only (~0.5 MB) - English, basic Western European
  European, // Latin + Cyrillic + Greek (~2-4 MB) - Full European language
            // support
  Asian, // European + CJK common (~10-20 MB) - Chinese/Japanese/Korean common
         // chars
  Full   // All available glyphs (~50-100 MB) - Maximum Unicode coverage
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
  static LanguageSupport ParseLanguageSupport(const std::string &str);

  // ---- Overlay ----
  bool overlayEnabled = true;   // Set to false to disable overlay entirely
  std::uint32_t toggleKey = 33; // F key (0x21)
  std::uint32_t controllerToggleButton =
      0x8000;                         // Y button on Xbox (0x8000 = 32768)
  float controllerScrollSpeed = 3.0f; // Scroll speed for controller thumbstick

  // ---- Font ----
  FontPreset fontPreset = FontPreset::Barlow;
  std::string customFontFile;
  float fontSize = 24.0f;      // Body text size
  float titleFontSize = 28.0f; // Title text size (0 = same as body)
  float titleScale =
      1.2f; // Scale multiplier for title (alternative to titleFontSize)
  LanguageSupport languageSupport =
      LanguageSupport::European; // Unicode glyph range coverage

  // ---- Window (percentage of screen size, 0-100) ----
  float windowWidthPercent = 50.0f;  // 50% of screen width
  float windowHeightPercent = 70.0f; // 70% of screen height
  float windowRounding = 0.0f;       // Corner rounding in pixels
  float windowPadding = 32.0f;       // Padding inside window

  // ---- Colors (RGB, 0-255) ----
  // Title text
  std::uint8_t titleColorR = 255;
  std::uint8_t titleColorG = 255;
  std::uint8_t titleColorB = 255;

  // Body text
  std::uint8_t bodyColorR = 255;
  std::uint8_t bodyColorG = 255;
  std::uint8_t bodyColorB = 255;

  // Window background
  std::uint8_t windowColorR = 0;
  std::uint8_t windowColorG = 0;
  std::uint8_t windowColorB = 0;

  // Window border
  std::uint8_t borderColorR = 160;
  std::uint8_t borderColorG = 160;
  std::uint8_t borderColorB = 160;
  float borderSize = 1.0f;

  // Separator line
  std::uint8_t separatorColorR = 140;
  std::uint8_t separatorColorG = 140;
  std::uint8_t separatorColorB = 140;

  // Scrollbar background
  std::uint8_t scrollbarBgColorR = 25;
  std::uint8_t scrollbarBgColorG = 25;
  std::uint8_t scrollbarBgColorB = 25;

  // Scrollbar thumb (handle)
  std::uint8_t scrollbarColorR = 160;
  std::uint8_t scrollbarColorG = 160;
  std::uint8_t scrollbarColorB = 160;

  // Scrollbar thumb hover
  std::uint8_t scrollbarHoverColorR = 210;
  std::uint8_t scrollbarHoverColorG = 210;
  std::uint8_t scrollbarHoverColorB = 210;

  // Scrollbar settings
  float scrollbarSize = 10.0f;    // Width of scrollbar
  float scrollbarRounding = 0.0f; // Rounding of scrollbar corners
  float scrollSpeed = 50.0f;      // Pixels per scroll wheel tick

  // ---- Visibility Toggles ----
  bool centerTitle = true;         // Center the title within the window
  bool showCornerOrnaments = true; // SkyUI message-box border artwork
  bool showTitle = true;           // Show book title
  bool showSeparator = true;       // Show separator line under title
  bool showBorder = true;          // Show window border
  bool showScrollbarTrack = true; // Show scrollbar background track

  // ---- Per-Element Transparency (0-1) ----
  float windowAlpha = 0.80f;        // Window background transparency
  float borderAlpha = 0.85f;         // Border transparency
  float separatorAlpha = 0.75f;      // Separator line transparency
  float scrollbarTrackAlpha = 0.50f; // Scrollbar background transparency
  float scrollbarThumbAlpha = 1.0f; // Scrollbar thumb transparency

private:
  Settings() = default;
  Settings(const Settings &) = delete;
  Settings(Settings &&) = delete;
  ~Settings() = default;

  Settings &operator=(const Settings &) = delete;
  Settings &operator=(Settings &&) = delete;
};

} // namespace Easy2Read
