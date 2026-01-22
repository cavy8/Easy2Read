#pragma once

#include <string>

namespace RE {
class TESObjectBOOK;
class TESQuest;
class BGSBaseAlias;
} // namespace RE

namespace Easy2Read {

class AliasResolver {
public:
  [[nodiscard]] static AliasResolver *GetSingleton();

  /**
   * Resolve all <Alias=...> tags in the input string.
   * Finds the quest that owns this book, then resolves aliases from that quest.
   *
   * @param text The input text potentially containing alias tags
   * @param book The book being displayed (required to find owning quest)
   * @return Text with alias tags replaced by resolved names
   */
  [[nodiscard]] std::string ResolveAliases(const std::string &text,
                                           RE::TESObjectBOOK *book = nullptr);

private:
  AliasResolver() = default;
  ~AliasResolver() = default;
  AliasResolver(const AliasResolver &) = delete;
  AliasResolver(AliasResolver &&) = delete;
  AliasResolver &operator=(const AliasResolver &) = delete;
  AliasResolver &operator=(AliasResolver &&) = delete;

  /**
   * Find the quest that has this book as an alias.
   *
   * @param book The book to find the owning quest for
   * @return The quest that owns this book, or nullptr if not found
   */
  RE::TESQuest *FindQuestForBook(RE::TESObjectBOOK *book);

  /**
   * Find an alias by name within a specific quest.
   *
   * @param quest The quest to search
   * @param aliasName The alias name to find
   * @return The alias, or nullptr if not found
   */
  RE::BGSBaseAlias *FindAliasInQuest(RE::TESQuest *quest,
                                     const std::string &aliasName);

  /**
   * Resolve an alias to its display name.
   *
   * @param alias The alias to resolve
   * @return The display name, or empty string if unresolved
   */
  std::string ResolveAliasName(RE::BGSBaseAlias *alias);
};

} // namespace Easy2Read
