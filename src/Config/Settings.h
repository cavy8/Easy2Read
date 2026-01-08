#pragma once

#include <cstdint>
#include <string>

namespace Easy2Read {

enum class FontPreset {
  Default,  // Roboto - clean, readable
  Dyslexic, // OpenDyslexic - accessibility font
  Custom    // User-specified font file
};

class Settings {
public:
  [[nodiscard]] static Settings *GetSingleton();

  void Load();

  // Get the resolved font file path based on current preset
  [[nodiscard]] std::string GetFontPath() const;

  // Convert string to FontPreset enum
  static FontPreset ParseFontPreset(const std::string &str);

  // ---- Hotkey ----
  std::uint32_t toggleKey = 33; // F key (0x21)

  // ---- Font ----
  FontPreset fontPreset = FontPreset::Default;
  std::string customFontFile;
  float fontSize = 18.0f;

  // ---- Window ----
  float windowWidth = 600.0f;
  float windowHeight = 400.0f;
  float backgroundAlpha = 0.95f;
  bool closeOnClickOutside = false;

  // Colors (RGBA)
  std::uint32_t textColor = 0xE0E0E0FF;
  std::uint32_t backgroundColor = 0x1A1A1AFF;

private:
  Settings() = default;
  Settings(const Settings &) = delete;
  Settings(Settings &&) = delete;
  ~Settings() = default;

  Settings &operator=(const Settings &) = delete;
  Settings &operator=(Settings &&) = delete;
};

} // namespace Easy2Read
