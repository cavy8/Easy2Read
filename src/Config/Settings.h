#pragma once

#include <cstdint>
#include <string>

namespace Easy2Read {

enum class FontPreset {
  Default,  // ImGui default font
  Dyslexic, // OpenDyslexic - accessibility font
  Custom    // User-specified font file
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
  FontPreset fontPreset = FontPreset::Default;
  std::string customFontFile;
  float fontSize = 24.0f;

  // ---- Window ----
  float windowWidth = 800.0f;
  float windowHeight = 600.0f;
  float windowOpacity = 0.90f; // 0-1

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
