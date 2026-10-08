#pragma once

#include <string>
#include <cstdint>
#include <optional>

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
   * Uses the opened note's stored quest and instance when available, otherwise
   * finds the quest that owns this book.
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
   * Resolve an alias using the note's stored instance before live names.
   * Location aliases require a stored name in the quest's instance text data.
   *
   * @param alias The alias to resolve
   * @return The display name, or empty string if unresolved
   */
  std::string ResolveAliasName(
      RE::BGSBaseAlias *alias,
      std::optional<std::uint32_t> instanceID = std::nullopt);
};

} // namespace Easy2Read
