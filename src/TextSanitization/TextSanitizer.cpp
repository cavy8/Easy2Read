#include "TextSanitizer.h"
#include "PCH.h"
#include <array>
#include <unordered_map>

namespace Easy2Read {

// AnyASCII-style transliteration table for common unsupported characters
static const std::unordered_map<uint32_t, std::string_view>
    kTransliterationTable = {
        // Curly quotes -> straight quotes (most common Skyrim tofu issue)
        {0x2018, "'"},  // LEFT SINGLE QUOTATION MARK
        {0x2019, "'"},  // RIGHT SINGLE QUOTATION MARK
        {0x201A, "'"},  // SINGLE LOW-9 QUOTATION MARK
        {0x201B, "'"},  // SINGLE HIGH-REVERSED-9 QUOTATION MARK
        {0x201C, "\""}, // LEFT DOUBLE QUOTATION MARK
        {0x201D, "\""}, // RIGHT DOUBLE QUOTATION MARK
        {0x201E, "\""}, // DOUBLE LOW-9 QUOTATION MARK
        {0x201F, "\""}, // DOUBLE HIGH-REVERSED-9 QUOTATION MARK
        {0x2032, "'"},  // PRIME
        {0x2033, "\""}, // DOUBLE PRIME
        {0x2035, "'"},  // REVERSED PRIME
        {0x2036, "\""}, // REVERSED DOUBLE PRIME
        {0x275B, "'"},  // HEAVY SINGLE TURNED COMMA QUOTATION MARK
        {0x275C, "'"},  // HEAVY SINGLE COMMA QUOTATION MARK
        {0x275D, "\""}, // HEAVY DOUBLE TURNED COMMA QUOTATION MARK
        {0x275E, "\""}, // HEAVY DOUBLE COMMA QUOTATION MARK

        // Dashes and hyphens
        {0x2010, "-"},   // HYPHEN
        {0x2011, "-"},   // NON-BREAKING HYPHEN
        {0x2012, "-"},   // FIGURE DASH
        {0x2013, "-"},   // EN DASH
        {0x2014, "--"},  // EM DASH
        {0x2015, "--"},  // HORIZONTAL BAR
        {0x2212, "-"},   // MINUS SIGN
        {0xFE58, "--"},  // SMALL EM DASH
        {0x2E3A, "--"},  // TWO-EM DASH
        {0x2E3B, "---"}, // THREE-EM DASH

        // Ellipsis and periods
        {0x2026, "..."}, // HORIZONTAL ELLIPSIS
        {0x22EF, "..."}, // MIDLINE HORIZONTAL ELLIPSIS

        // Spaces
        {0x00A0, " "}, // NO-BREAK SPACE
        {0x2000, " "}, // EN QUAD
        {0x2001, " "}, // EM QUAD
        {0x2002, " "}, // EN SPACE
        {0x2003, " "}, // EM SPACE
        {0x2004, " "}, // THREE-PER-EM SPACE
        {0x2005, " "}, // FOUR-PER-EM SPACE
        {0x2006, " "}, // SIX-PER-EM SPACE
        {0x2007, " "}, // FIGURE SPACE
        {0x2008, " "}, // PUNCTUATION SPACE
        {0x2009, " "}, // THIN SPACE
        {0x200A, " "}, // HAIR SPACE
        {0x200B, ""},  // ZERO WIDTH SPACE (remove)
        {0x202F, " "}, // NARROW NO-BREAK SPACE
        {0x205F, " "}, // MEDIUM MATHEMATICAL SPACE
        {0x3000, " "}, // IDEOGRAPHIC SPACE
        {0xFEFF, ""},  // ZERO WIDTH NO-BREAK SPACE / BOM (remove)

        // Common symbols
        {0x2022, "*"}, // BULLET
        {0x2023, "*"}, // TRIANGULAR BULLET
        {0x2043, "-"}, // HYPHEN BULLET
        {0x25E6, "o"}, // WHITE BULLET
        {0x2219, "*"}, // BULLET OPERATOR
        {0x00B7, "*"}, // MIDDLE DOT (if unsupported)
        {0x2024, "."}, // ONE DOT LEADER
        {0x2027, "-"}, // HYPHENATION POINT

        // Arrows (common in mod text)
        {0x2190, "<-"},  // LEFTWARDS ARROW
        {0x2191, "^"},   // UPWARDS ARROW
        {0x2192, "->"},  // RIGHTWARDS ARROW
        {0x2193, "v"},   // DOWNWARDS ARROW
        {0x2194, "<->"}, // LEFT RIGHT ARROW
        {0x21D0, "<="},  // LEFTWARDS DOUBLE ARROW
        {0x21D2, "=>"},  // RIGHTWARDS DOUBLE ARROW

        // Math symbols
        {0x00D7, "x"},        // MULTIPLICATION SIGN
        {0x2217, "*"},        // ASTERISK OPERATOR
        {0x2264, "<="},       // LESS-THAN OR EQUAL TO
        {0x2265, ">="},       // GREATER-THAN OR EQUAL TO
        {0x2260, "!="},       // NOT EQUAL TO
        {0x00B1, "+/-"},      // PLUS-MINUS SIGN
        {0x2248, "~="},       // ALMOST EQUAL TO
        {0x221E, "infinity"}, // INFINITY

        // Trademark and copyright (common in mod descriptions)
        {0x00AE, "(R)"},  // REGISTERED SIGN (if unsupported)
        {0x2122, "(TM)"}, // TRADE MARK SIGN
        {0x00A9, "(C)"},  // COPYRIGHT SIGN (if unsupported)

        // Fractions (if unsupported)
        {0x00BD, "1/2"}, // VULGAR FRACTION ONE HALF
        {0x2153, "1/3"}, // VULGAR FRACTION ONE THIRD
        {0x2154, "2/3"}, // VULGAR FRACTION TWO THIRDS
        {0x00BC, "1/4"}, // VULGAR FRACTION ONE QUARTER
        {0x00BE, "3/4"}, // VULGAR FRACTION THREE QUARTERS
};

// Windows-1252 (CP1252) high bytes 0x80-0x9F translation
// These are NOT valid UTF-8 but commonly appear in Skyrim text
static const std::array<std::string_view, 32> kCP1252Table = {{
    "EUR",  // 0x80 - Euro sign
    "",     // 0x81 - undefined
    ",",    // 0x82 - Single low-9 quotation mark
    "f",    // 0x83 - Latin small letter f with hook
    ",,",   // 0x84 - Double low-9 quotation mark
    "...",  // 0x85 - Horizontal ellipsis
    "+",    // 0x86 - Dagger
    "++",   // 0x87 - Double dagger
    "^",    // 0x88 - Modifier letter circumflex accent
    "%",    // 0x89 - Per mille sign
    "S",    // 0x8A - Latin capital letter S with caron
    "<",    // 0x8B - Single left-pointing angle quotation mark
    "OE",   // 0x8C - Latin capital ligature OE
    "",     // 0x8D - undefined
    "Z",    // 0x8E - Latin capital letter Z with caron
    "",     // 0x8F - undefined
    "",     // 0x90 - undefined
    "'",    // 0x91 - Left single quotation mark
    "'",    // 0x92 - Right single quotation mark
    "\"",   // 0x93 - Left double quotation mark
    "\"",   // 0x94 - Right double quotation mark
    "*",    // 0x95 - Bullet
    "-",    // 0x96 - En dash
    "--",   // 0x97 - Em dash
    "~",    // 0x98 - Small tilde
    "(TM)", // 0x99 - Trade mark sign
    "s",    // 0x9A - Latin small letter s with caron
    ">",    // 0x9B - Single right-pointing angle quotation mark
    "oe",   // 0x9C - Latin small ligature oe
    "",     // 0x9D - undefined
    "z",    // 0x9E - Latin small letter z with caron
    "Y",    // 0x9F - Latin capital letter Y with diaeresis
}};

TextSanitizer *TextSanitizer::GetSingleton() {
  static TextSanitizer singleton;
  return &singleton;
}

TextSanitizer::TextSanitizer() { InitializeSupportedChars(); }

void TextSanitizer::InitializeSupportedChars() {
  // Valid characters from Tofu-Detective (Skyrim's validNameChars):
  // `1234567890-=~!@#$%^&*():_+QWERTYUIOP[]ASDFGHJKL;'"ZXCVBNM,./
  // qwertyuiop{}\asdfghjklzxcvbnm<>?|¡¢£¤¥¦§¨©ª«®¯°²³´¶·¸¹º»¼½¾¿
  // ÄÀÁÂÃÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ

  const std::string_view validChars = "`1234567890-=~!@#$%^&*():_+QWERTYUIOP[]"
                                      "ASDFGHJKL;'\"ZXCVBNM,./"
                                      "qwertyuiop{}\\asdfghjklzxcvbnm<>?| "
                                      "\xC2\xA1\xC2\xA2\xC2\xA3\xC2\xA4\xC2\xA5"
                                      "\xC2\xA6\xC2\xA7\xC2\xA8" // ¡¢£¤¥¦§¨
                                      "\xC2\xA9\xC2\xAA\xC2\xAB\xC2\xAE\xC2\xAF"
                                      "\xC2\xB0\xC2\xB2\xC2\xB3" // ©ª«®¯°²³
                                      "\xC2\xB4\xC2\xB6\xC2\xB7\xC2\xB8\xC2\xB9"
                                      "\xC2\xBA\xC2\xBB\xC2\xBC" // ´¶·¸¹º»¼
                                      "\xC2\xBD\xC2\xBE\xC2\xBF" // ½¾¿
                                      "\xC3\x84\xC3\x80\xC3\x81\xC3\x82\xC3\x83"
                                      "\xC3\x85\xC3\x86\xC3\x87" // ÄÀÁÂÃÅÆÇ
                                      "\xC3\x88\xC3\x89\xC3\x8A\xC3\x8B\xC3\x8C"
                                      "\xC3\x8D\xC3\x8E\xC3\x8F" // ÈÉÊËÌÍÎÏ
                                      "\xC3\x90\xC3\x91\xC3\x92\xC3\x93\xC3\x94"
                                      "\xC3\x95\xC3\x96\xC3\x97" // ÐÑÒÓÔÕÖ×
                                      "\xC3\x98\xC3\x99\xC3\x9A\xC3\x9B\xC3\x9C"
                                      "\xC3\x9D\xC3\x9E\xC3\x9F" // ØÙÚÛÜÝÞß
                                      "\xC3\xA0\xC3\xA1\xC3\xA2\xC3\xA3\xC3\xA4"
                                      "\xC3\xA5\xC3\xA6\xC3\xA7" // àáâãäåæç
                                      "\xC3\xA8\xC3\xA9\xC3\xAA\xC3\xAB\xC3\xAC"
                                      "\xC3\xAD\xC3\xAE\xC3\xAF" // èéêëìíîï
                                      "\xC3\xB0\xC3\xB1\xC3\xB2\xC3\xB3\xC3\xB4"
                                      "\xC3\xB5\xC3\xB6\xC3\xB7" // ðñòóôõö÷
                                      "\xC3\xB8\xC3\xB9\xC3\xBA\xC3\xBB\xC3\xBC"
                                      "\xC3\xBD\xC3\xBE\xC3\xBF"; // øùúûüýþÿ

  // Decode UTF-8 and add each codepoint to the supported set
  size_t i = 0;
  while (i < validChars.size()) {
    uint32_t codepoint = 0;
    unsigned char c = static_cast<unsigned char>(validChars[i]);

    if (c < 0x80) {
      // ASCII
      codepoint = c;
      i += 1;
    } else if ((c & 0xE0) == 0xC0 && i + 1 < validChars.size()) {
      // 2-byte UTF-8
      codepoint = (c & 0x1F) << 6;
      codepoint |= (static_cast<unsigned char>(validChars[i + 1]) & 0x3F);
      i += 2;
    } else if ((c & 0xF0) == 0xE0 && i + 2 < validChars.size()) {
      // 3-byte UTF-8
      codepoint = (c & 0x0F) << 12;
      codepoint |= (static_cast<unsigned char>(validChars[i + 1]) & 0x3F) << 6;
      codepoint |= (static_cast<unsigned char>(validChars[i + 2]) & 0x3F);
      i += 3;
    } else if ((c & 0xF8) == 0xF0 && i + 3 < validChars.size()) {
      // 4-byte UTF-8
      codepoint = (c & 0x07) << 18;
      codepoint |= (static_cast<unsigned char>(validChars[i + 1]) & 0x3F) << 12;
      codepoint |= (static_cast<unsigned char>(validChars[i + 2]) & 0x3F) << 6;
      codepoint |= (static_cast<unsigned char>(validChars[i + 3]) & 0x3F);
      i += 4;
    } else {
      // Invalid UTF-8, skip byte
      i += 1;
      continue;
    }

    supportedChars_.insert(codepoint);
  }

  // Also add common control characters that should pass through
  supportedChars_.insert('\t'); // Tab
  supportedChars_.insert('\n'); // Newline
  supportedChars_.insert('\r'); // Carriage return

  SKSE::log::info("TextSanitizer: Initialized with {} supported characters",
                  supportedChars_.size());
}

bool TextSanitizer::IsSupported(uint32_t codepoint) const {
  return supportedChars_.count(codepoint) > 0;
}

std::string_view TextSanitizer::GetReplacement(uint32_t codepoint) const {
  auto it = kTransliterationTable.find(codepoint);
  if (it != kTransliterationTable.end()) {
    return it->second;
  }
  // Return empty to indicate no mapping - caller should pass through original
  return "";
}

bool TextSanitizer::NeedsSanitization(std::string_view input) const {
  if (!enabled_ || mode_ == SanitizationMode::Off) {
    return false;
  }

  size_t i = 0;
  while (i < input.size()) {
    uint32_t codepoint = 0;
    unsigned char c = static_cast<unsigned char>(input[i]);

    if (c < 0x80) {
      // ASCII - almost always supported
      if (!IsSupported(c)) {
        return true;
      }
      i += 1;
    } else if ((c & 0xE0) == 0xC0 && i + 1 < input.size()) {
      codepoint = (c & 0x1F) << 6;
      codepoint |= (static_cast<unsigned char>(input[i + 1]) & 0x3F);
      if (!IsSupported(codepoint)) {
        return true;
      }
      i += 2;
    } else if ((c & 0xF0) == 0xE0 && i + 2 < input.size()) {
      codepoint = (c & 0x0F) << 12;
      codepoint |= (static_cast<unsigned char>(input[i + 1]) & 0x3F) << 6;
      codepoint |= (static_cast<unsigned char>(input[i + 2]) & 0x3F);
      if (!IsSupported(codepoint)) {
        return true;
      }
      i += 3;
    } else if ((c & 0xF8) == 0xF0 && i + 3 < input.size()) {
      codepoint = (c & 0x07) << 18;
      codepoint |= (static_cast<unsigned char>(input[i + 1]) & 0x3F) << 12;
      codepoint |= (static_cast<unsigned char>(input[i + 2]) & 0x3F) << 6;
      codepoint |= (static_cast<unsigned char>(input[i + 3]) & 0x3F);
      if (!IsSupported(codepoint)) {
        return true;
      }
      i += 4;
    } else {
      // Invalid UTF-8 sequence
      return true;
    }
  }

  return false;
}

std::string TextSanitizer::Sanitize(std::string_view input) const {
  if (!enabled_ || mode_ == SanitizationMode::Off) {
    return std::string(input);
  }

  // Fast path: if no sanitization needed, return input as-is
  if (!NeedsSanitization(input)) {
    return std::string(input);
  }

  const size_t maxOutputSize = static_cast<size_t>(
      static_cast<float>(input.size()) * maxExpansionRatio_);

  std::string result;
  result.reserve(input.size());

  // Track if we're inside angle brackets (e.g., <Alias=Player>)
  // Content inside these should be preserved as-is, but only if both < and >
  // exist
  bool insideAngleBrackets = false;

  // Check if the string has matching angle brackets (both < and >)
  bool hasMatchingBrackets = (input.find('<') != std::string_view::npos) &&
                             (input.find('>') != std::string_view::npos);

  size_t i = 0;
  while (i < input.size()) {
    uint32_t codepoint = 0;
    size_t charLen = 1;
    unsigned char c = static_cast<unsigned char>(input[i]);

    if (c < 0x80) {
      codepoint = c;
      charLen = 1;

      // Check for angle bracket boundaries (only if matching brackets exist)
      if (hasMatchingBrackets) {
        if (c == '<') {
          insideAngleBrackets = true;
          result += static_cast<char>(c);
          i += 1;
          continue;
        } else if (c == '>') {
          insideAngleBrackets = false;
          result += static_cast<char>(c);
          i += 1;
          continue;
        }

        // If inside angle brackets, just copy the byte and continue
        if (insideAngleBrackets) {
          result += static_cast<char>(c);
          i += 1;
          continue;
        }
      }
    } else if ((c & 0xE0) == 0xC0 && i + 1 < input.size()) {
      codepoint = (c & 0x1F) << 6;
      codepoint |= (static_cast<unsigned char>(input[i + 1]) & 0x3F);
      charLen = 2;
      // If inside angle brackets, copy and skip sanitization
      if (insideAngleBrackets) {
        result.append(input.substr(i, charLen));
        i += charLen;
        continue;
      }
    } else if ((c & 0xF0) == 0xE0 && i + 2 < input.size()) {
      codepoint = (c & 0x0F) << 12;
      codepoint |= (static_cast<unsigned char>(input[i + 1]) & 0x3F) << 6;
      codepoint |= (static_cast<unsigned char>(input[i + 2]) & 0x3F);
      charLen = 3;
      // If inside angle brackets, copy and skip sanitization
      if (insideAngleBrackets) {
        result.append(input.substr(i, charLen));
        i += charLen;
        continue;
      }
    } else if ((c & 0xF8) == 0xF0 && i + 3 < input.size()) {
      codepoint = (c & 0x07) << 18;
      codepoint |= (static_cast<unsigned char>(input[i + 1]) & 0x3F) << 12;
      codepoint |= (static_cast<unsigned char>(input[i + 2]) & 0x3F) << 6;
      codepoint |= (static_cast<unsigned char>(input[i + 3]) & 0x3F);
      charLen = 4;
      // If inside angle brackets, copy and skip sanitization
      if (insideAngleBrackets) {
        result.append(input.substr(i, charLen));
        i += charLen;
        continue;
      }
    } else {
      // Not valid UTF-8 multibyte start - check if it's Windows-1252 (CP1252)
      // CP1252 uses bytes 0x80-0x9F for special characters
      if (c >= 0x80 && c <= 0x9F) {
        // Windows-1252 character - use CP1252 translation table
        std::string_view replacement = kCP1252Table[c - 0x80];
        if (debugMode_ || logReplacements_) {
          SKSE::log::info("TextSanitizer: CP1252 byte 0x{:02X} -> '{}'", c,
                          replacement);
        }

        if (mode_ == SanitizationMode::AnyASCII && !replacement.empty()) {
          result.append(replacement);
        } else if (mode_ == SanitizationMode::DetectOnly) {
          result += static_cast<char>(c);
        }
      } else if (c >= 0xA0 && c <= 0xFF) {
        // Latin-1 Supplement (0xA0-0xFF) - these map directly to Unicode
        // U+00A0-U+00FF Check if they're in our supported set
        if (IsSupported(c)) {
          result += static_cast<char>(c);
        } else {
          // Try to transliterate
          auto it = kTransliterationTable.find(c);
          if (it != kTransliterationTable.end()) {
            if (mode_ == SanitizationMode::AnyASCII) {
              result.append(it->second);
            }
          } else if (mode_ == SanitizationMode::AnyASCII) {
            // No mapping found - pass through unchanged
            result += static_cast<char>(c);
          }
        }
      } else {
        // Truly invalid byte - pass through unchanged
        SKSE::log::debug("TextSanitizer: Unknown byte 0x{:02X} at position {}, "
                         "passing through",
                         c, i);
        result += static_cast<char>(c);
      }
      i += 1;
      continue;
    }

    if (IsSupported(codepoint)) {
      // Character is supported, copy original bytes
      result.append(input.substr(i, charLen));
    } else {
      // Character not supported, needs replacement
      if (mode_ == SanitizationMode::AnyASCII) {
        std::string_view replacement = GetReplacement(codepoint);

        // If no mapping, pass through unchanged
        if (replacement.empty()) {
          result.append(input.substr(i, charLen));
          SKSE::log::debug(
              "TextSanitizer: No mapping for U+{:04X}, passing through",
              codepoint);
        } else {
          SKSE::log::debug("TextSanitizer: Replacing U+{:04X} -> '{}'",
                           codepoint, replacement);
          // Check expansion limit
          if (result.size() + replacement.size() <= maxOutputSize) {
            result.append(replacement);
          }
        }
      } else if (mode_ == SanitizationMode::DetectOnly) {
        // DetectOnly: log but keep original
        result.append(input.substr(i, charLen));
        if (debugMode_ || logReplacements_) {
          SKSE::log::info("TextSanitizer: Detected unsupported U+{:04X}",
                          codepoint);
        }
      }
    }

    i += charLen;
  }

  return result;
}

} // namespace Easy2Read
