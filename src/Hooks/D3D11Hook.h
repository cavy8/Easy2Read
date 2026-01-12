#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <functional>

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

  static HRESULT WINAPI HookedPresent(IDXGISwapChain *pSwapChain,
                                      UINT SyncInterval, UINT Flags);

  bool InitImGui(IDXGISwapChain *swapChain);
  void RenderImGui();

  RenderCallback renderCallback;

  ID3D11Device *device = nullptr;
  ID3D11DeviceContext *context = nullptr;
  ID3D11RenderTargetView *renderTargetView = nullptr;
  IDXGISwapChain *swapChain_ = nullptr;

  bool initialized = false;
  bool imguiInitialized = false;
  bool usingD3D12Fallback = false;

  // Original function pointer
  inline static decltype(&HookedPresent) originalPresent = nullptr;
};

} // namespace Easy2Read
