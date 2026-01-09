#pragma once

#include <atomic>

namespace Easy2Read {

class InputHandler : public RE::BSTEventSink<RE::InputEvent *> {
public:
  [[nodiscard]] static InputHandler *GetSingleton();

  void Register();
  void Unregister();

  // Event handler
  RE::BSEventNotifyControl
  ProcessEvent(RE::InputEvent *const *a_event,
               RE::BSTEventSource<RE::InputEvent *> *a_eventSource) override;

  // Check if overlay should be shown
  [[nodiscard]] bool IsOverlayVisible() const { return overlayVisible.load(); }

  // Toggle overlay visibility
  void ToggleOverlay();
  void ShowOverlay();
  void HideOverlay();

private:
  InputHandler() = default;
  InputHandler(const InputHandler &) = delete;
  InputHandler(InputHandler &&) = delete;
  ~InputHandler() override = default;

  InputHandler &operator=(const InputHandler &) = delete;
  InputHandler &operator=(InputHandler &&) = delete;

  std::atomic<bool> overlayVisible{false};
};

} // namespace Easy2Read
