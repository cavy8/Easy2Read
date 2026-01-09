#include "BookUtils.h"
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

  // Trim trailing whitespace
  while (!result.empty() && (result.back() == ' ' || result.back() == '\n')) {
    result.pop_back();
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
