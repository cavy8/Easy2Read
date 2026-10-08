#include "MenuControlsHook.h"
#include "Config/Settings.h"
#include "MenuWatcher.h"
#include "PCH.h"
#include "UI/Overlay.h"
#include "UI/MenuFramework.h"

namespace Easy2Read {

RE::BSEventNotifyControl MenuControlsHook::ProcessEvent_Hook(
    RE::MenuControls *a_this, RE::InputEvent *const *a_event,
    RE::BSTEventSource<RE::InputEvent *> *a_eventSource) {

  auto *overlay = Overlay::GetSingleton();
  auto *menuWatcher = MenuWatcher::GetSingleton();
  auto *settings = Settings::GetSingleton();

  // Only block inputs when overlay is visible AND book menu is open
  bool shouldBlock = overlay && overlay->IsVisible() && menuWatcher &&
                     menuWatcher->IsBookMenuOpen() && !MenuFramework::IsInputCaptured();

  if (shouldBlock && a_event) {
    // Iterate through the event chain and process/block inputs
    for (auto *event = *a_event; event; event = event->next) {
      if (!event) {
        continue;
      }

      const auto eventType = event->GetEventType();

      // Handle thumbstick: capture for scrolling, then zero to block game
      if (eventType == RE::INPUT_EVENT_TYPE::kThumbstick) {
        auto *thumbstick = static_cast<RE::ThumbstickEvent *>(event);

        // Right thumbstick (ID 1) is used for scrolling the overlay
        if (thumbstick->IsRight()) {
          float yAxis = thumbstick->yValue;
          // Apply deadzone and send to overlay for scrolling
          if (std::abs(yAxis) > 0.2f) {
            float scrollDelta =
                yAxis * settings->controllerScrollSpeed * 0.016f;
            overlay->AddScrollInput(scrollDelta);
          }
        }

        // Zero out both thumbsticks to prevent game from using them
        thumbstick->xValue = 0.0f;
        thumbstick->yValue = 0.0f;
      }

      // Route mouse movement to the overlay's cursor instead of the book
      if (eventType == RE::INPUT_EVENT_TYPE::kMouseMove) {
        auto *move = static_cast<RE::MouseMoveEvent *>(event);
        overlay->AddMouseMove(static_cast<float>(move->mouseInputX),
                              static_cast<float>(move->mouseInputY));
        move->mouseInputX = 0;
        move->mouseInputY = 0;
      }

      // Block mouse buttons and wheel (prevents page turning). Buttons go to
      // the overlay so the scrollbar can be dragged; the wheel scrolls via
      // InputHandler, which reads only the scan code (8 = up, 9 = down).
      if (eventType == RE::INPUT_EVENT_TYPE::kButton) {
        auto *button = static_cast<RE::ButtonEvent *>(event);
        if (button->device.get() == RE::INPUT_DEVICE::kMouse) {
          if (button->IsDown() || button->IsUp()) {
            overlay->QueueMouseButton(button->GetIDCode(), button->IsDown());
          }
          // Zero out the button value to prevent it from registering
          button->value = 0.0f;
          button->heldDownSecs = 0.0f;
        }
      }
    }
  }

  // Call the original function
  return _ProcessEvent(a_this, a_event, a_eventSource);
}

void MenuControlsHook::Install() {
  // Hook MenuControls::ProcessEvent (vtable index 1)
  // BSTEventSink<InputEvent*> is the first parent class
  // Index 0 = destructor, Index 1 = ProcessEvent
  REL::Relocation<std::uintptr_t> vtable(RE::VTABLE_MenuControls[0]);

  _ProcessEvent = vtable.write_vfunc(0x1, ProcessEvent_Hook);

  SKSE::log::info(
      "MenuControlsHook: Installed (blocks input when overlay visible)");
}

} // namespace Easy2Read
