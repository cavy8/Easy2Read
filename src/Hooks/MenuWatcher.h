#pragma once

#include <atomic>
#include <string>

namespace Easy2Read {

class MenuWatcher : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
public:
  [[nodiscard]] static MenuWatcher *GetSingleton();

  void Register();
  void Unregister();

  // Event handler
  RE::BSEventNotifyControl ProcessEvent(
      const RE::MenuOpenCloseEvent *a_event,
      RE::BSTEventSource<RE::MenuOpenCloseEvent> *a_eventSource) override;

  // State accessors
  [[nodiscard]] bool IsBookMenuOpen() const { return bookMenuOpen.load(); }
  [[nodiscard]] RE::TESObjectBOOK *GetCurrentBook() const {
    return currentBook;
  }
  [[nodiscard]] const std::string &GetCurrentBookTitle() const {
    return bookTitle;
  }
  [[nodiscard]] const std::string &GetCurrentBookText() const {
    return bookText;
  }

  // Clear cached book data
  void ClearBookCache();

private:
  MenuWatcher() = default;
  MenuWatcher(const MenuWatcher &) = delete;
  MenuWatcher(MenuWatcher &&) = delete;
  ~MenuWatcher() override = default;

  MenuWatcher &operator=(const MenuWatcher &) = delete;
  MenuWatcher &operator=(MenuWatcher &&) = delete;

  // Extract book data when menu opens
  void CacheBookData();

  std::atomic<bool> bookMenuOpen{false};
  RE::TESObjectBOOK *currentBook{nullptr};
  std::string bookTitle;
  std::string bookText;
};

} // namespace Easy2Read
