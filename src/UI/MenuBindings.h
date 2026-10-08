#pragma once

#include "ThirdParty/SKSEMenuFramework.h"
#include <optional>

namespace Easy2Read::MenuFramework {
struct BindingCode {
  std::uint32_t code;
  ImGuiMCP::ImGuiKey key;
};
// Match the framework's Input.cpp translation while retaining existing INI codes.
inline constexpr BindingCode keyboardBindings[] = {
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kTab), ImGuiMCP::ImGuiKey_Tab},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kLeft), ImGuiMCP::ImGuiKey_LeftArrow},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kRight), ImGuiMCP::ImGuiKey_RightArrow},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kUp), ImGuiMCP::ImGuiKey_UpArrow},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kDown), ImGuiMCP::ImGuiKey_DownArrow},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kPageUp), ImGuiMCP::ImGuiKey_PageUp},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kPageDown), ImGuiMCP::ImGuiKey_PageDown},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kHome), ImGuiMCP::ImGuiKey_Home},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kEnd), ImGuiMCP::ImGuiKey_End},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kInsert), ImGuiMCP::ImGuiKey_Insert},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kDelete), ImGuiMCP::ImGuiKey_Delete},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kBackspace), ImGuiMCP::ImGuiKey_Backspace},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kSpacebar), ImGuiMCP::ImGuiKey_Space},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kEnter), ImGuiMCP::ImGuiKey_Enter},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kEscape), ImGuiMCP::ImGuiKey_Escape},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum0), ImGuiMCP::ImGuiKey_0},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum1), ImGuiMCP::ImGuiKey_1},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum2), ImGuiMCP::ImGuiKey_2},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum3), ImGuiMCP::ImGuiKey_3},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum4), ImGuiMCP::ImGuiKey_4},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum5), ImGuiMCP::ImGuiKey_5},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum6), ImGuiMCP::ImGuiKey_6},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum7), ImGuiMCP::ImGuiKey_7},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum8), ImGuiMCP::ImGuiKey_8},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNum9), ImGuiMCP::ImGuiKey_9},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kA), ImGuiMCP::ImGuiKey_A},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kB), ImGuiMCP::ImGuiKey_B},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kC), ImGuiMCP::ImGuiKey_C},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kD), ImGuiMCP::ImGuiKey_D},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kE), ImGuiMCP::ImGuiKey_E},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF), ImGuiMCP::ImGuiKey_F},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kG), ImGuiMCP::ImGuiKey_G},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kH), ImGuiMCP::ImGuiKey_H},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kI), ImGuiMCP::ImGuiKey_I},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kJ), ImGuiMCP::ImGuiKey_J},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kK), ImGuiMCP::ImGuiKey_K},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kL), ImGuiMCP::ImGuiKey_L},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kM), ImGuiMCP::ImGuiKey_M},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kN), ImGuiMCP::ImGuiKey_N},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kO), ImGuiMCP::ImGuiKey_O},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kP), ImGuiMCP::ImGuiKey_P},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kQ), ImGuiMCP::ImGuiKey_Q},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kR), ImGuiMCP::ImGuiKey_R},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kS), ImGuiMCP::ImGuiKey_S},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kT), ImGuiMCP::ImGuiKey_T},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kU), ImGuiMCP::ImGuiKey_U},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kV), ImGuiMCP::ImGuiKey_V},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kW), ImGuiMCP::ImGuiKey_W},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kX), ImGuiMCP::ImGuiKey_X},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kY), ImGuiMCP::ImGuiKey_Y},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kZ), ImGuiMCP::ImGuiKey_Z},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF1), ImGuiMCP::ImGuiKey_F1},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF2), ImGuiMCP::ImGuiKey_F2},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF3), ImGuiMCP::ImGuiKey_F3},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF4), ImGuiMCP::ImGuiKey_F4},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF5), ImGuiMCP::ImGuiKey_F5},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF6), ImGuiMCP::ImGuiKey_F6},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF7), ImGuiMCP::ImGuiKey_F7},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF8), ImGuiMCP::ImGuiKey_F8},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF9), ImGuiMCP::ImGuiKey_F9},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF10), ImGuiMCP::ImGuiKey_F10},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF11), ImGuiMCP::ImGuiKey_F11},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kF12), ImGuiMCP::ImGuiKey_F12},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kApostrophe), ImGuiMCP::ImGuiKey_Apostrophe},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kComma), ImGuiMCP::ImGuiKey_Comma},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kMinus), ImGuiMCP::ImGuiKey_Minus},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kPeriod), ImGuiMCP::ImGuiKey_Period},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kSlash), ImGuiMCP::ImGuiKey_Slash},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kSemicolon), ImGuiMCP::ImGuiKey_Semicolon},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kEquals), ImGuiMCP::ImGuiKey_Equal},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kBracketLeft), ImGuiMCP::ImGuiKey_LeftBracket},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kBackslash), ImGuiMCP::ImGuiKey_Backslash},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kBracketRight), ImGuiMCP::ImGuiKey_RightBracket},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kTilde), ImGuiMCP::ImGuiKey_GraveAccent},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kCapsLock), ImGuiMCP::ImGuiKey_CapsLock},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kScrollLock), ImGuiMCP::ImGuiKey_ScrollLock},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kNumLock), ImGuiMCP::ImGuiKey_NumLock},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kPrintScreen), ImGuiMCP::ImGuiKey_PrintScreen},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kPause), ImGuiMCP::ImGuiKey_Pause},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_0), ImGuiMCP::ImGuiKey_Keypad0},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_1), ImGuiMCP::ImGuiKey_Keypad1},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_2), ImGuiMCP::ImGuiKey_Keypad2},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_3), ImGuiMCP::ImGuiKey_Keypad3},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_4), ImGuiMCP::ImGuiKey_Keypad4},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_5), ImGuiMCP::ImGuiKey_Keypad5},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_6), ImGuiMCP::ImGuiKey_Keypad6},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_7), ImGuiMCP::ImGuiKey_Keypad7},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_8), ImGuiMCP::ImGuiKey_Keypad8},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_9), ImGuiMCP::ImGuiKey_Keypad9},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_Decimal), ImGuiMCP::ImGuiKey_KeypadDecimal},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_Divide), ImGuiMCP::ImGuiKey_KeypadDivide},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_Multiply), ImGuiMCP::ImGuiKey_KeypadMultiply},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_Subtract), ImGuiMCP::ImGuiKey_KeypadSubtract},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_Plus), ImGuiMCP::ImGuiKey_KeypadAdd},
    {static_cast<std::uint32_t>(RE::BSKeyboardDevice::Key::kKP_Enter), ImGuiMCP::ImGuiKey_KeypadEnter},
};
inline constexpr BindingCode controllerBindings[] = {
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kUp), ImGuiMCP::ImGuiKey_GamepadDpadUp},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kDown), ImGuiMCP::ImGuiKey_GamepadDpadDown},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kLeft), ImGuiMCP::ImGuiKey_GamepadDpadLeft},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kRight), ImGuiMCP::ImGuiKey_GamepadDpadRight},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kStart), ImGuiMCP::ImGuiKey_GamepadStart},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kBack), ImGuiMCP::ImGuiKey_GamepadBack},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kLeftThumb), ImGuiMCP::ImGuiKey_GamepadL3},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kRightThumb), ImGuiMCP::ImGuiKey_GamepadR3},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kLeftShoulder), ImGuiMCP::ImGuiKey_GamepadL1},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kRightShoulder), ImGuiMCP::ImGuiKey_GamepadR1},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kLeftTrigger), ImGuiMCP::ImGuiKey_GamepadL2},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kRightTrigger), ImGuiMCP::ImGuiKey_GamepadR2},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kA), ImGuiMCP::ImGuiKey_GamepadFaceDown},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kB), ImGuiMCP::ImGuiKey_GamepadFaceRight},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kX), ImGuiMCP::ImGuiKey_GamepadFaceLeft},
    {static_cast<std::uint32_t>(RE::BSWin32GamepadDevice::Key::kY), ImGuiMCP::ImGuiKey_GamepadFaceUp},
};
inline ImGuiMCP::ImGuiKey ToPickerKey(std::uint32_t code, bool controller) {
  const auto *entries = controller ? controllerBindings : keyboardBindings;
  const auto count = controller ? std::size(controllerBindings) : std::size(keyboardBindings);
  for (std::size_t i = 0; i < count; ++i) {
    if (entries[i].code == code) return entries[i].key;
  }
  return ImGuiMCP::ImGuiKey_None;
}
inline std::optional<std::uint32_t> FromPickerKey(ImGuiMCP::ImGuiKey key, bool controller) {
  if (key == ImGuiMCP::ImGuiKey_None) return 0; // Clear disables this binding.
  const auto *entries = controller ? controllerBindings : keyboardBindings;
  const auto count = controller ? std::size(controllerBindings) : std::size(keyboardBindings);
  for (std::size_t i = 0; i < count; ++i) {
    if (entries[i].key == key) return entries[i].code;
  }
  return std::nullopt;
}
} // namespace Easy2Read::MenuFramework
