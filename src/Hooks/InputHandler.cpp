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
  auto settings = Settings::GetSingleton();

  // If overlay is disabled in settings, don't process overlay-related input
  if (!settings->overlayEnabled) {
    return RE::BSEventNotifyControl::kContinue;
  }

  // If BookMenu closed, hide overlay
  if (!menuWatcher->IsBookMenuOpen()) {
    if (overlay->IsVisible()) {
      overlay->Hide();
    }
    return RE::BSEventNotifyControl::kContinue;
  }

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

    // Handle controller right thumbstick for scrolling
    if (event->eventType == RE::INPUT_EVENT_TYPE::kThumbstick &&
        event->device.get() == RE::INPUT_DEVICE::kGamepad) {
      auto thumbstickEvent = static_cast<RE::ThumbstickEvent *>(event);

      // Right thumbstick (ID 1) for scrolling
      if (thumbstickEvent->IsRight() && overlay->IsVisible()) {
        float yAxis = thumbstickEvent->yValue;
        // Apply deadzone
        if (std::abs(yAxis) > 0.2f) {
          float scrollDelta = yAxis * settings->controllerScrollSpeed *
                              0.016f; // ~60fps frame time
          overlay->AddScrollInput(scrollDelta);
        }
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

    // Keyboard toggle
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
            overlay->SetContent(title, text);
          }
          overlay->Show();
        }
      }
    }

    // Controller toggle - configurable button (default: Y button = 0x8000)
    if (buttonEvent->device == RE::INPUT_DEVICE::kGamepad) {
      // Debug: log gamepad button presses (trace level = debug only)
      SKSE::log::trace("Gamepad button: 0x{:X} ({}), looking for 0x{:X}",
                       keyCode, keyCode, settings->controllerToggleButton);

      if (keyCode == settings->controllerToggleButton) {
        SKSE::log::trace("Controller toggle button matched");

        if (overlay->IsVisible()) {
          overlay->Hide();
        } else {
          auto book = menuWatcher->GetCurrentBook();
          if (book) {
            std::string title = BookUtils::GetBookTitle(book);
            std::string text = BookUtils::GetBookText(book);
            overlay->SetContent(title, text);
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
