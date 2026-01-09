#include "MenuWatcher.h"
#include "PCH.h"
#include "Utils/BookUtils.h"


namespace Easy2Read {

MenuWatcher *MenuWatcher::GetSingleton() {
  static MenuWatcher singleton;
  return &singleton;
}

void MenuWatcher::Register() {
  if (auto ui = RE::UI::GetSingleton()) {
    ui->AddEventSink<RE::MenuOpenCloseEvent>(this);
    SKSE::log::info("MenuWatcher registered for menu events");
  } else {
    SKSE::log::error("Failed to get UI singleton for MenuWatcher registration");
  }
}

void MenuWatcher::Unregister() {
  if (auto ui = RE::UI::GetSingleton()) {
    ui->RemoveEventSink<RE::MenuOpenCloseEvent>(this);
    SKSE::log::info("MenuWatcher unregistered");
  }
}

RE::BSEventNotifyControl MenuWatcher::ProcessEvent(
    const RE::MenuOpenCloseEvent *a_event,
    [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>
        *a_eventSource) {

  if (!a_event) {
    return RE::BSEventNotifyControl::kContinue;
  }

  // Check if this is the BookMenu
  if (a_event->menuName == RE::BookMenu::MENU_NAME) {
    if (a_event->opening) {
      SKSE::log::info("BookMenu opened");
      bookMenuOpen.store(true);
      CacheBookData();
    } else {
      SKSE::log::info("BookMenu closed");
      bookMenuOpen.store(false);
      ClearBookCache();
    }
  }

  return RE::BSEventNotifyControl::kContinue;
}

void MenuWatcher::CacheBookData() {
  // Get the book being displayed using the static helper
  currentBook = RE::BookMenu::GetTargetForm();
  if (!currentBook) {
    SKSE::log::warn("Cannot cache book data: GetTargetForm returned null");
    return;
  }

  // Use BookUtils for proper text extraction with markup stripping
  bookTitle = BookUtils::GetBookTitle(currentBook);
  bookText = BookUtils::GetBookText(currentBook);

  bool isNote = BookUtils::IsNote(currentBook);

  SKSE::log::info("  Book: {} (FormID: {:08X})", bookTitle,
                  currentBook->GetFormID());
  SKSE::log::info("  Type: {}", isNote ? "Note" : "Book");
  SKSE::log::info("  Clean text length: {} characters", bookText.length());
}

void MenuWatcher::ClearBookCache() {
  currentBook = nullptr;
  bookTitle.clear();
  bookText.clear();
  SKSE::log::debug("Book cache cleared");
}

} // namespace Easy2Read
