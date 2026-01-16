#pragma once

#include "PCH.h"

namespace Easy2Read {

/**
 * Hook for MenuControls::ProcessEvent to block inputs when overlay is visible.
 * Prevents page turning and camera movement while reading in the overlay.
 */
struct MenuControlsHook {
  static RE::BSEventNotifyControl
  ProcessEvent_Hook(RE::MenuControls *a_this, RE::InputEvent *const *a_event,
                    RE::BSTEventSource<RE::InputEvent *> *a_eventSource);

  static void Install();

  static inline REL::Relocation<decltype(&ProcessEvent_Hook)> _ProcessEvent;
};

} // namespace Easy2Read
