#include "BookPromptIcon.h"
#include "PCH.h"
#include <WICTextureLoader.h>

namespace Easy2Read {
namespace {
std::string GetIconName(std::uint32_t button, bool gamepad) {
  if (gamepad) {
    switch (button) {
    case 0x0001: return "Up";
    case 0x0002: return "Down";
    case 0x0004: return "Left";
    case 0x0008: return "Right";
    case 0x0010: return "360_Start";
    case 0x0020: return "360_Back";
    case 0x0040: return "360_L3";
    case 0x0080: return "360_R3";
    case 0x0100: return "360_LB";
    case 0x0200: return "360_RB";
    case 0x1000: return "360_A";
    case 0x2000: return "360_B";
    case 0x4000: return "360_X";
    case 0x8000: return "360_Y";
    default: return {};
    }
  }
  // DirectInput scan codes, matching InputHandler and ToggleKey in the INI.
  if (button >= 0x02 && button <= 0x0B) {
    return std::string(1, "1234567890"[button - 0x02]);
  }
  if (button >= 0x10 && button <= 0x19) {
    return std::string(1, "QWERTYUIOP"[button - 0x10]);
  }
  if (button >= 0x1E && button <= 0x26) {
    return std::string(1, "ASDFGHJKL"[button - 0x1E]);
  }
  if (button >= 0x2C && button <= 0x32) {
    return std::string(1, "ZXCVBNM"[button - 0x2C]);
  }
  if (button >= 0x3B && button <= 0x44) {
    return fmt::format("F{}", button - 0x3A);
  }
  switch (button) {
  case 0x01: return "Esc";
  case 0x0C: return "Hyphen";
  case 0x0D: return "Equal";
  case 0x0E: return "Backspace";
  case 0x0F: return "Tab";
  case 0x1A: return "Bracketleft";
  case 0x1B: return "Bracketright";
  case 0x1C: return "Enter";
  case 0x1D: return "L-Ctrl";
  case 0x27: return "Semicolon";
  case 0x28: return "Quotesingle";
  case 0x29: return "Tilde";
  case 0x2A: return "L-Shift";
  case 0x2B: return "Backslash";
  case 0x33: return "Comma";
  case 0x34: return "Period";
  case 0x35: return "Slash";
  case 0x36: return "R-Shift";
  case 0x37: return "NumPadMult";
  case 0x38: return "L-Alt";
  case 0x39: return "Space";
  case 0x3A: return "CapsLock";
  case 0x45: return "NumLock";
  case 0x46: return "ScrollLock";
  case 0x47: return "Keypad7";
  case 0x48: return "Keypad8";
  case 0x49: return "NumPad9";
  case 0x4A: return "NumPadMinus";
  case 0x4B: return "Keypad4";
  case 0x4C: return "Keypad5";
  case 0x4D: return "Keypad6";
  case 0x4E: return "NumPadPlus";
  case 0x4F: return "Keypad1";
  case 0x50: return "Keypad2";
  case 0x51: return "Keypad3";
  case 0x52: return "NumPad0";
  case 0x53: return "NumPadDec";
  case 0x57: return "F11";
  case 0x58: return "F12";
  case 0x9C: return "KeypadEnter";
  case 0x9D: return "R-Ctrl";
  case 0xB5: return "NumPadDivide";
  case 0xB7: return "PrintScreen";
  case 0xB8: return "R-Alt";
  case 0xC5: return "Pause";
  case 0xC7: return "Home";
  case 0xC8: return "Up";
  case 0xC9: return "PgUp";
  case 0xCB: return "Left";
  case 0xCD: return "Right";
  case 0xCF: return "End";
  case 0xD0: return "Down";
  case 0xD1: return "PgDn";
  case 0xD2: return "Insert";
  case 0xD3: return "Delete";
  default: return {};
  }
}
} // namespace

void BookPromptIcon::Load(std::uint32_t button, bool gamepad) {
  texture.Reset();
  aspectRatio = 1.0f;
  const auto name = GetIconName(button, gamepad);
  if (name.empty()) {
    return;
  }
  const auto path = std::filesystem::path("Data/Interface/ImGuiIcons/Icons") /
                    (name + ".png");
  auto renderer = RE::BSGraphics::Renderer::GetSingleton();
  auto device = renderer ? reinterpret_cast<ID3D11Device *>(
      renderer->GetRuntimeData().forwarder) : nullptr;
  if (!device) {
    return;
  }
  const HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  Microsoft::WRL::ComPtr<ID3D11Resource> resource;
  const HRESULT result = DirectX::CreateWICTextureFromFile(
      device, path.c_str(), resource.GetAddressOf(), texture.GetAddressOf());
  if (SUCCEEDED(comResult)) {
    CoUninitialize();
  }
  if (FAILED(result)) {
    SKSE::log::warn("Could not load ImGui Icons art {} (HRESULT {:08X}); using text badge",
                    path.string(), static_cast<std::uint32_t>(result));
    return;
  }
  Microsoft::WRL::ComPtr<ID3D11Texture2D> image;
  if (SUCCEEDED(resource.As(&image))) {
    D3D11_TEXTURE2D_DESC description{};
    image->GetDesc(&description);
    aspectRatio = static_cast<float>(description.Width) /
                   static_cast<float>(description.Height);
  }
}

} // namespace Easy2Read
