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
  static const std::regex aliasPattern(R"(<Alias=([^>]+)>)",
                                       std::regex::icase);
  if (!std::regex_search(text, aliasPattern)) {
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
        if (refAlias->fillType.get() == RE::BGSBaseAlias::FILL_TYPE::kForced) {
          auto forcedHandle = refAlias->fillData.forced.forcedRef;
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
      const char *name = ref->GetDisplayFullName();
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
  }

  // Quest text stores named forms by alias ID, including locations. Use only
  // the current instance so a restarted radiant quest cannot reuse old names.
  if (auto *quest = alias->owningQuest) {
    for (auto *instance : quest->instanceData) {
      if (!instance || instance->id != quest->currentInstanceID) {
        continue;
      }

      for (const auto &entry : instance->stringData) {
        if (entry.aliasID != alias->aliasID) {
          continue;
        }

        auto *form = RE::TESForm::LookupByID(entry.fullNameFormID);
        const char *name = form ? form->GetName() : nullptr;
        if (name && name[0] != '\0') {
          return name;
        }
      }
      break;
    }
  }

  SKSE::log::debug("AliasResolver: No live or stored name for alias '{}'",
                   alias->aliasName.c_str());
  return "";
}

} // namespace Easy2Read
