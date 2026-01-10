#pragma once

#include <string>
#include <string_view>
#include <unordered_set>

namespace Easy2Read {

/**
 * Modes for text sanitization behavior.
 */
enum class SanitizationMode {
  Off,        // Disabled - no text modification
  DetectOnly, // Log unsupported chars but don't modify text
  AnyASCII    // Full replacement of unsupported chars
};

/**
 * Per-hook enable settings structure.
 */
struct HookSettings {
  bool enableDescription = true;
  bool enableDialogue = true;
  bool enableDialogueMenu = true;
  bool enableQuest = true;
  bool enableMapMarker = true;
  bool enableNpcName = true;
};

/**
 * Runtime text sanitizer that detects and replaces unsupported Unicode
 * characters. Uses the Tofu-Detective valid character set for detection and
 * AnyASCII-style transliteration for replacement.
 */
class TextSanitizer {
public:
  [[nodiscard]] static TextSanitizer *GetSingleton();

  /**
   * Sanitize a string by replacing unsupported Unicode characters.
   * @param input The input string to sanitize
   * @return Sanitized string with unsupported chars replaced
   */
  [[nodiscard]] std::string Sanitize(std::string_view input) const;

  /**
   * Fast check if a string contains any unsupported characters.
   * @param input The string to check
   * @return true if sanitization would modify the string
   */
  [[nodiscard]] bool NeedsSanitization(std::string_view input) const;

  /**
   * Check if a specific Unicode codepoint is supported by Skyrim's fonts.
   * @param codepoint The Unicode codepoint to check
   * @return true if the character is supported
   */
  [[nodiscard]] bool IsSupported(uint32_t codepoint) const;

  /**
   * Get the ASCII replacement for an unsupported codepoint.
   * @param codepoint The Unicode codepoint to transliterate
   * @return ASCII replacement string, or "?" if no mapping exists
   */
  [[nodiscard]] std::string_view GetReplacement(uint32_t codepoint) const;

  // Configuration
  void SetEnabled(bool enabled) { enabled_ = enabled; }
  [[nodiscard]] bool IsEnabled() const { return enabled_; }

  void SetMode(SanitizationMode mode) { mode_ = mode; }
  [[nodiscard]] SanitizationMode GetMode() const { return mode_; }

  void SetLogReplacements(bool log) { logReplacements_ = log; }
  [[nodiscard]] bool GetLogReplacements() const { return logReplacements_; }

  void SetMaxExpansionRatio(float ratio) { maxExpansionRatio_ = ratio; }
  [[nodiscard]] float GetMaxExpansionRatio() const {
    return maxExpansionRatio_;
  }

  // Debug mode
  void SetDebugMode(bool debug) { debugMode_ = debug; }
  [[nodiscard]] bool IsDebugMode() const { return debugMode_; }

  // Hook settings
  void SetHookSettings(const HookSettings &settings) { hooks_ = settings; }
  [[nodiscard]] const HookSettings &GetHookSettings() const { return hooks_; }

private:
  TextSanitizer();
  ~TextSanitizer() = default;
  TextSanitizer(const TextSanitizer &) = delete;
  TextSanitizer(TextSanitizer &&) = delete;
  TextSanitizer &operator=(const TextSanitizer &) = delete;
  TextSanitizer &operator=(TextSanitizer &&) = delete;

  void InitializeSupportedChars();
  void InitializeTransliterationTable();

  // Set of codepoints supported by Skyrim's fonts
  std::unordered_set<uint32_t> supportedChars_;

  // Configuration
  bool enabled_ = true;
  SanitizationMode mode_ = SanitizationMode::AnyASCII;
  bool logReplacements_ = false;
  float maxExpansionRatio_ = 3.0f;
  bool debugMode_ = false;
  HookSettings hooks_;
};

} // namespace Easy2Read
