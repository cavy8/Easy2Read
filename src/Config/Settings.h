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

  // ---- Hotkey ----
  std::uint32_t toggleKey = 33; // F key (0x21)

  // ---- Font ----
  FontPreset fontPreset = FontPreset::Sovngarde;
  std::string customFontFile;
  float fontSize = 24.0f;

  // ---- Window (percentage of screen size, 0-100) ----
  float windowWidthPercent = 50.0f;  // 50% of screen width
  float windowHeightPercent = 70.0f; // 70% of screen height
  float windowOpacity = 0.90f;       // 0-1

  // ---- Colors (RGB, 0-255) ----
  std::uint8_t titleColorR = 255;
  std::uint8_t titleColorG = 220;
  std::uint8_t titleColorB = 150;

  std::uint8_t bodyColorR = 255;
  std::uint8_t bodyColorG = 255;
  std::uint8_t bodyColorB = 255;

  std::uint8_t windowColorR = 20;
  std::uint8_t windowColorG = 20;
  std::uint8_t windowColorB = 25;

private:
  Settings() = default;
  Settings(const Settings &) = delete;
  Settings(Settings &&) = delete;
  ~Settings() = default;

  Settings &operator=(const Settings &) = delete;
  Settings &operator=(Settings &&) = delete;
};

} // namespace Easy2Read
