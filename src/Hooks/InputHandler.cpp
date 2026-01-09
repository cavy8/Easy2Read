#include "InputHandler.h"
#include "Config/Settings.h"
#include "MenuWatcher.h"
#include "PCH.h"
#include "UI/Overlay.h"
#include "Utils/BookUtils.h"

namespace Easy2Read {

InputHandler *InputHandler::GetSingleton() {
  static InputHandler singleton;
  return &singleton;
}

void InputHandler::Register() {
  if (auto inputDeviceManager = RE::BSInputDeviceManager::GetSingleton()) {
    inputDeviceManager->AddEventSink(this);
    SKSE::log::info("InputHandler registered for input events");
  } else {
    SKSE::log::error(
        "Failed to get BSInputDeviceManager for InputHandler registration");
  }
}

void InputHandler::Unregister() {
  if (auto inputDeviceManager = RE::BSInputDeviceManager::GetSingleton()) {
    inputDeviceManager->RemoveEventSink(this);
    SKSE::log::info("InputHandler unregistered");
  }
}

RE::BSEventNotifyControl InputHandler::ProcessEvent(
    RE::InputEvent *const *a_event,
    [[maybe_unused]] RE::BSTEventSource<RE::InputEvent *> *a_eventSource) {

  if (!a_event) {
    return RE::BSEventNotifyControl::kContinue;
  }

  auto overlay = Overlay::GetSingleton();
  auto menuWatcher = MenuWatcher::GetSingleton();

  // If BookMenu closed, hide overlay
  if (!menuWatcher->IsBookMenuOpen()) {
    if (overlay->IsVisible()) {
      overlay->Hide();
    }
    return RE::BSEventNotifyControl::kContinue;
  }

  auto settings = Settings::GetSingleton();

  for (auto event = *a_event; event; event = event->next) {
    // Handle mouse scroll wheel when overlay is visible
    if (event->eventType == RE::INPUT_EVENT_TYPE::kButton &&
        event->device.get() == RE::INPUT_DEVICE::kMouse) {
      auto buttonEvent = static_cast<RE::ButtonEvent *>(event);
      std::uint32_t scanCode = buttonEvent->GetIDCode();

      // Mouse wheel events: 8 = scroll up, 9 = scroll down
      if (overlay->IsVisible() && (scanCode == 8 || scanCode == 9)) {
        float wheelDelta = (scanCode == 8) ? 1.0f : -1.0f;
        overlay->AddScrollInput(wheelDelta);
        SKSE::log::trace("Mouse wheel: scanCode={}, delta={}", scanCode,
                         wheelDelta);
      }
    }

    if (event->eventType != RE::INPUT_EVENT_TYPE::kButton) {
      continue;
    }

    auto buttonEvent = static_cast<RE::ButtonEvent *>(event);

    if (!buttonEvent->IsDown()) {
      continue;
    }

    std::uint32_t keyCode = buttonEvent->GetIDCode();

    if (buttonEvent->device == RE::INPUT_DEVICE::kKeyboard) {
      if (keyCode == settings->toggleKey) {
        SKSE::log::info("Toggle hotkey pressed (key {})", keyCode);

        if (overlay->IsVisible()) {
          overlay->Hide();
        } else {
          // Set content and show overlay
          auto book = menuWatcher->GetCurrentBook();
          if (book) {
            std::string title = BookUtils::GetBookTitle(book);
            std::string text = BookUtils::GetBookText(book);
            bool isNote = BookUtils::IsNote(book);
            overlay->SetContent(title, text, isNote);
          }
          overlay->Show();
        }
      }
    }
  }

  return RE::BSEventNotifyControl::kContinue;
}

void InputHandler::ToggleOverlay() { Overlay::GetSingleton()->Toggle(); }

void InputHandler::ShowOverlay() { Overlay::GetSingleton()->Show(); }

void InputHandler::HideOverlay() { Overlay::GetSingleton()->Hide(); }

} // namespace Easy2Read
