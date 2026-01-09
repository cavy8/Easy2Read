#pragma once

#include <string>

namespace RE {
class TESObjectBOOK;
}

namespace Easy2Read {

class BookUtils {
public:
  // Extract book title
  static std::string GetBookTitle(RE::TESObjectBOOK *book);

  // Extract book text content, with markup stripped
  static std::string GetBookText(RE::TESObjectBOOK *book);

  // Strip Scaleform/HTML-like tags from text
  static std::string StripMarkup(const std::string &text);

  // Check if book is a note (single page, no cover)
  static bool IsNote(RE::TESObjectBOOK *book);

private:
  // Internal helper to clean up whitespace
  static std::string NormalizeWhitespace(const std::string &text);

  // Strip [pagebreak] markers
  static std::string StripPagebreaks(const std::string &text);
};

} // namespace Easy2Read
