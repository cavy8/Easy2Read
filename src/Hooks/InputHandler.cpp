#include "InputHandler.h"
#include "Config/Settings.h"
#include "MenuWatcher.h"
#include "PCH.h"


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

  // Only process input when BookMenu is open
  auto menuWatcher = MenuWatcher::GetSingleton();
  if (!menuWatcher->IsBookMenuOpen()) {
    // If overlay is visible but menu closed, hide it
    if (overlayVisible.load()) {
      HideOverlay();
    }
    return RE::BSEventNotifyControl::kContinue;
  }

  auto settings = Settings::GetSingleton();

  for (auto event = *a_event; event; event = event->next) {
    // Only handle button events
    if (event->eventType != RE::INPUT_EVENT_TYPE::kButton) {
      continue;
    }

    auto buttonEvent = static_cast<RE::ButtonEvent *>(event);

    // Only respond to key down (not held or released)
    if (!buttonEvent->IsDown()) {
      continue;
    }

    // Check if this is our configured hotkey
    std::uint32_t keyCode = buttonEvent->GetIDCode();

    // Handle keyboard input
    if (buttonEvent->device == RE::INPUT_DEVICE::kKeyboard) {
      if (keyCode == settings->toggleKey) {
        SKSE::log::info("Toggle hotkey pressed (key {})", keyCode);
        ToggleOverlay();
      }
    }
  }

  return RE::BSEventNotifyControl::kContinue;
}

void InputHandler::ToggleOverlay() {
  if (overlayVisible.load()) {
    HideOverlay();
  } else {
    ShowOverlay();
  }
}

void InputHandler::ShowOverlay() {
  if (!overlayVisible.load()) {
    overlayVisible.store(true);
    SKSE::log::info("Overlay shown");
  }
}

void InputHandler::HideOverlay() {
  if (overlayVisible.load()) {
    overlayVisible.store(false);
    SKSE::log::info("Overlay hidden");
  }
}

} // namespace Easy2Read
