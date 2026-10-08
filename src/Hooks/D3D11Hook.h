#pragma once

#include "PCH.h"
#include <d3d11.h>
#include <functional>

struct ImGuiContext;

namespace Easy2Read {

class D3D11Hook {
public:
  using RenderCallback = std::function<void()>;

  [[nodiscard]] static D3D11Hook *GetSingleton();

  bool Install();
  void Uninstall();

  void SetRenderCallback(RenderCallback callback);

  [[nodiscard]] bool IsInitialized() const { return initialized; }

private:
  D3D11Hook() = default;
  D3D11Hook(const D3D11Hook &) = delete;
  D3D11Hook(D3D11Hook &&) = delete;
  ~D3D11Hook() = default;

  D3D11Hook &operator=(const D3D11Hook &) = delete;
  D3D11Hook &operator=(D3D11Hook &&) = delete;

  static void HookedPostDisplay(RE::BookMenu *menu);

  bool InitImGui();
  void RenderImGui();

  RenderCallback renderCallback;

  ID3D11Device *device = nullptr;
  ID3D11DeviceContext *context = nullptr;
  ImGuiContext *imguiContext = nullptr;

  bool initialized = false;
  bool imguiInitialized = false;

  // Original function pointer
  inline static REL::Relocation<decltype(&HookedPostDisplay)> originalPostDisplay;
};

} // namespace Easy2Read
