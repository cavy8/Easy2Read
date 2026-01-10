#include "BookUtils.h"
#include "ImageMappings.h"
#include "PCH.h"


namespace Easy2Read {

std::string BookUtils::GetBookTitle(RE::TESObjectBOOK *book) {
  if (!book) {
    return "";
  }

  const char *name = book->GetName();
  return name ? name : "";
}

std::string BookUtils::GetBookText(RE::TESObjectBOOK *book) {
  if (!book) {
    return "";
  }

  RE::BSString description;
  book->GetDescription(description, nullptr);

  std::string rawText = description.c_str();

  // Strip markup and normalize whitespace
  std::string cleanText = StripMarkup(rawText);
  cleanText = StripPagebreaks(cleanText);
  return NormalizeWhitespace(cleanText);
}

std::string BookUtils::StripMarkup(const std::string &text) {
  std::string result;
  result.reserve(text.size());

  bool inTag = false;
  bool inEntity = false;
  std::string entityBuffer;

  for (size_t i = 0; i < text.size(); ++i) {
    char c = text[i];

    if (inEntity) {
      entityBuffer += c;
      if (c == ';') {
        // Resolve common HTML entities
        if (entityBuffer == "&nbsp;") {
          result += ' ';
        } else if (entityBuffer == "&lt;") {
          result += '<';
        } else if (entityBuffer == "&gt;") {
          result += '>';
        } else if (entityBuffer == "&amp;") {
          result += '&';
        } else if (entityBuffer == "&quot;") {
          result += '"';
        } else if (entityBuffer == "&apos;") {
          result += '\'';
        } else if (entityBuffer == "&#10;" || entityBuffer == "&#13;") {
          result += '\n';
        } else {
          // Unknown entity, just skip it
        }
        inEntity = false;
        entityBuffer.clear();
      }
      continue;
    }

    if (c == '&') {
      inEntity = true;
      entityBuffer = "&";
      continue;
    }

    if (c == '<') {
      // Check for img tag
      if (i + 4 < text.size()) {
        std::string tagCheck = text.substr(i, 4);
        if (tagCheck == "<img" || tagCheck == "<IMG") {
          // Find the end of the tag
          size_t tagEnd = text.find('>', i);
          if (tagEnd != std::string::npos) {
            // Extract the full tag
            std::string fullTag = text.substr(i, tagEnd - i + 1);

            // Find src attribute
            size_t srcPos = fullTag.find("src=");
            if (srcPos == std::string::npos) {
              srcPos = fullTag.find("SRC=");
            }

            if (srcPos != std::string::npos) {
              // Find the quote after src=
              size_t quoteStart = fullTag.find_first_of("'\"", srcPos);
              if (quoteStart != std::string::npos) {
                char quoteChar = fullTag[quoteStart];
                size_t quoteEnd = fullTag.find(quoteChar, quoteStart + 1);
                if (quoteEnd != std::string::npos) {
                  std::string srcValue =
                      fullTag.substr(quoteStart + 1, quoteEnd - quoteStart - 1);

                  // Remove img:// prefix if present
                  if (srcValue.substr(0, 6) == "img://") {
                    srcValue = srcValue.substr(6);
                  }

                  // Look up replacement
                  auto *imageMappings = ImageMappings::GetSingleton();
                  std::string replacement =
                      imageMappings->GetReplacement(srcValue);

                  if (!replacement.empty()) {
                    result += replacement;
                  }
                  // If no mapping, the image is just removed (no output)
                }
              }
            }

            // Skip past the entire img tag
            i = tagEnd;
            continue;
          }
        }
      }

      // Check for special tags that should become newlines
      if (i + 3 < text.size()) {
        std::string tagStart = text.substr(i, 4);
        if (tagStart == "<br>" || tagStart == "<BR>" || tagStart == "<br/" ||
            tagStart == "<BR/") {
          result += '\n';
        }
        // Check for </p> or </P> - paragraph end
        if (i + 3 < text.size() &&
            (text.substr(i, 4) == "</p>" || text.substr(i, 4) == "</P>")) {
          result += "\n\n";
        }
      }
      inTag = true;
      continue;
    }

    if (c == '>') {
      inTag = false;
      continue;
    }

    if (!inTag) {
      result += c;
    }
  }

  return result;
}

std::string BookUtils::NormalizeWhitespace(const std::string &text) {
  std::string result;
  result.reserve(text.size());

  bool lastWasSpace = false;
  bool lastWasNewline = false;
  int consecutiveNewlines = 0;

  for (char c : text) {
    if (c == '\n' || c == '\r') {
      if (!lastWasNewline) {
        consecutiveNewlines = 1;
        lastWasNewline = true;
      } else if (consecutiveNewlines < 2) {
        consecutiveNewlines++;
      }
      lastWasSpace = false;
      continue;
    }

    // Flush pending newlines
    if (lastWasNewline) {
      for (int i = 0; i < consecutiveNewlines; ++i) {
        result += '\n';
      }
      lastWasNewline = false;
      consecutiveNewlines = 0;
    }

    if (c == ' ' || c == '\t') {
      if (!lastWasSpace && !result.empty()) {
        result += ' ';
        lastWasSpace = true;
      }
    } else {
      result += c;
      lastWasSpace = false;
    }
  }

  // Trim leading whitespace
  size_t start = 0;
  while (start < result.size() &&
         (result[start] == ' ' || result[start] == '\n' ||
          result[start] == '\r' || result[start] == '\t')) {
    start++;
  }
  if (start > 0) {
    result = result.substr(start);
  }

  // Trim trailing whitespace
  while (!result.empty() && (result.back() == ' ' || result.back() == '\n')) {
    result.pop_back();
  }

  return result;
}

std::string BookUtils::StripPagebreaks(const std::string &text) {
  std::string result = text;

  // Remove [pagebreak] markers (case insensitive)
  const std::string pagebreakLower = "[pagebreak]";
  const std::string pagebreakUpper = "[PAGEBREAK]";
  const std::string pagebreakMixed = "[Pagebreak]";

  size_t pos;
  while ((pos = result.find(pagebreakLower)) != std::string::npos) {
    result.erase(pos, pagebreakLower.length());
  }
  while ((pos = result.find(pagebreakUpper)) != std::string::npos) {
    result.erase(pos, pagebreakUpper.length());
  }
  while ((pos = result.find(pagebreakMixed)) != std::string::npos) {
    result.erase(pos, pagebreakMixed.length());
  }

  return result;
}

bool BookUtils::IsNote(RE::TESObjectBOOK *book) {
  if (!book) {
    return false;
  }

  // Notes have the kNote flag set in their data
  return book->IsNote();
}

} // namespace Easy2Read
