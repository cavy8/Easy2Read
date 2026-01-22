#include "AliasResolver.h"
#include "PCH.h"
#include <regex>

namespace Easy2Read {

AliasResolver *AliasResolver::GetSingleton() {
  static AliasResolver singleton;
  return &singleton;
}

std::string AliasResolver::ResolveAliases(const std::string &text,
                                          RE::TESObjectBOOK *book) {
  // Fast path: if no alias tags, return as-is
  if (text.find("<Alias=") == std::string::npos &&
      text.find("<alias=") == std::string::npos) {
    return text;
  }

  if (!book) {
    SKSE::log::debug("AliasResolver: No book provided, cannot resolve aliases");
    return text;
  }

  // Find the quest that has this book as an alias
  RE::TESQuest *owningQuest = FindQuestForBook(book);
  if (!owningQuest) {
    SKSE::log::debug("AliasResolver: No quest found for book '{}'",
                     book->GetName());
    return text;
  }

  SKSE::log::info("AliasResolver: Found quest '{}' for book '{}'",
                  owningQuest->GetName(), book->GetName());

  std::string result = text;

  // Regex to match <Alias=NAME> pattern (case insensitive for "Alias")
  std::regex aliasPattern(R"(<[Aa]lias=([^>]+)>)");

  std::smatch match;
  std::string::const_iterator searchStart = result.cbegin();
  std::string processedResult;
  size_t lastPos = 0;

  while (std::regex_search(searchStart, result.cend(), match, aliasPattern)) {
    // Append text before the match
    size_t matchPos = match.position(0) + (searchStart - result.cbegin());
    processedResult.append(result, lastPos, matchPos - lastPos);

    std::string aliasName = match[1].str();
    SKSE::log::debug("AliasResolver: Found alias tag: <Alias={}>", aliasName);

    // Find the alias in the owning quest
    RE::BGSBaseAlias *alias = FindAliasInQuest(owningQuest, aliasName);

    if (alias) {
      std::string resolvedName = ResolveAliasName(alias);
      if (!resolvedName.empty()) {
        SKSE::log::info("AliasResolver: Resolved <Alias={}> -> '{}'", aliasName,
                        resolvedName);
        processedResult.append(resolvedName);
      } else {
        // Couldn't resolve, keep original tag
        processedResult.append(match[0].str());
        SKSE::log::debug("AliasResolver: Alias '{}' not filled", aliasName);
      }
    } else {
      // Alias not found in quest, keep original tag
      processedResult.append(match[0].str());
      SKSE::log::debug("AliasResolver: Alias '{}' not found in quest",
                       aliasName);
    }

    lastPos = matchPos + match.length(0);
    searchStart = match.suffix().first;
  }

  // Append remaining text after last match
  processedResult.append(result, lastPos, result.length() - lastPos);

  return processedResult;
}

RE::TESQuest *AliasResolver::FindQuestForBook(RE::TESObjectBOOK *book) {
  if (!book) {
    return nullptr;
  }

  auto *dataHandler = RE::TESDataHandler::GetSingleton();
  if (!dataHandler) {
    return nullptr;
  }

  // Get the book's form ID to compare
  RE::FormID bookFormID = book->GetFormID();

  // Search all quests for one that has this book as an alias
  auto &quests = dataHandler->GetFormArray<RE::TESQuest>();

  for (auto *quest : quests) {
    if (!quest) {
      continue;
    }

    // Check each alias in the quest
    for (auto *alias : quest->aliases) {
      if (!alias) {
        continue;
      }

      // Check if this is a reference alias
      if (auto *refAlias = skyrim_cast<RE::BGSRefAlias *>(alias)) {
        // Check if the alias is filled with this book
        auto *ref = refAlias->GetReference();
        if (ref) {
          auto *baseObj = ref->GetBaseObject();
          if (baseObj && baseObj->GetFormID() == bookFormID) {
            return quest;
          }
          // Also check if the reference itself matches
          if (ref->GetFormID() == bookFormID) {
            return quest;
          }
        }

        // Check if the alias has a forced reference to this book's base form
        // Some quests use forced refs rather than filled refs
        auto forcedHandle = refAlias->fillData.forced.forcedRef;
        if (forcedHandle) {
          auto forcedRef =
              RE::TESObjectREFR::LookupByHandle(forcedHandle.native_handle());
          if (forcedRef) {
            auto *baseObj = forcedRef->GetBaseObject();
            if (baseObj && baseObj->GetFormID() == bookFormID) {
              return quest;
            }
          }
        }
      }
    }
  }

  return nullptr;
}

RE::BGSBaseAlias *
AliasResolver::FindAliasInQuest(RE::TESQuest *quest,
                                const std::string &aliasName) {
  if (!quest) {
    return nullptr;
  }

  for (auto *alias : quest->aliases) {
    if (!alias) {
      continue;
    }

    const char *name = alias->aliasName.c_str();
    if (name && aliasName == name) {
      return alias;
    }
  }

  return nullptr;
}

std::string AliasResolver::ResolveAliasName(RE::BGSBaseAlias *alias) {
  if (!alias) {
    return "";
  }

  // Try to cast to reference alias (most common type)
  if (auto *refAlias = skyrim_cast<RE::BGSRefAlias *>(alias)) {
    auto *ref = refAlias->GetReference();
    if (ref) {
      const char *name = ref->GetName();
      if (name && name[0] != '\0') {
        return name;
      }
      // Fallback to base object name
      auto *baseObj = ref->GetBaseObject();
      if (baseObj) {
        name = baseObj->GetName();
        if (name && name[0] != '\0') {
          return name;
        }
      }
    }
    return "";
  }

  // Note: BGSLocAlias is not fully reverse-engineered in CommonLibSSE-NG
  // Location aliases cannot be resolved at this time

  // Unknown alias type
  SKSE::log::debug("AliasResolver: Unknown alias type for '{}'",
                   alias->aliasName.c_str());
  return "";
}

} // namespace Easy2Read
