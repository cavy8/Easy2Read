#include "D3D11Hook.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "PCH.h"
#include "UI/Overlay.h"
#include <imgui.h>


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

namespace Easy2Read {

D3D11Hook *D3D11Hook::GetSingleton() {
  static D3D11Hook singleton;
  return &singleton;
}

bool D3D11Hook::Install() {
  SKSE::log::info("Installing D3D11 hook...");

  // Get the render window
  auto renderWindow = RE::BSGraphics::Renderer::GetSingleton();
  if (!renderWindow) {
    SKSE::log::error("Failed to get BSGraphics::Renderer singleton");
    return false;
  }

  auto &renderData = renderWindow->data;
  auto swapChain = renderData.renderWindows[0].swapChain;
  if (!swapChain) {
    SKSE::log::error("Failed to get swap chain");
    return false;
  }

  // Get the vtable of the swap chain
  void **vtable = *reinterpret_cast<void ***>(swapChain);

  // Present is at index 8 in the IDXGISwapChain vtable
  void *presentAddr = vtable[8];

  SKSE::log::info("SwapChain::Present at {:p}", presentAddr);

  // Save original function pointer
  originalPresent = reinterpret_cast<decltype(originalPresent)>(presentAddr);

  // Patch the vtable directly
  DWORD oldProtect;
  if (!VirtualProtect(&vtable[8], sizeof(void *), PAGE_EXECUTE_READWRITE,
                      &oldProtect)) {
    SKSE::log::error("Failed to change vtable memory protection");
    return false;
  }

  vtable[8] = reinterpret_cast<void *>(&HookedPresent);

  VirtualProtect(&vtable[8], sizeof(void *), oldProtect, &oldProtect);

  initialized = true;
  SKSE::log::info("D3D11 hook installed successfully");
  return true;
}

void D3D11Hook::Uninstall() {
  if (imguiInitialized) {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    imguiInitialized = false;
  }

  if (renderTargetView) {
    renderTargetView->Release();
    renderTargetView = nullptr;
  }

  initialized = false;
  SKSE::log::info("D3D11 hook uninstalled");
}

void D3D11Hook::SetRenderCallback(RenderCallback callback) {
  renderCallback = std::move(callback);
}

HRESULT WINAPI D3D11Hook::HookedPresent(IDXGISwapChain *pSwapChain,
                                        UINT SyncInterval, UINT Flags) {
  auto hook = GetSingleton();

  if (!hook->imguiInitialized) {
    hook->InitImGui(pSwapChain);
  }

  hook->RenderImGui();

  return originalPresent(pSwapChain, SyncInterval, Flags);
}

bool D3D11Hook::InitImGui(IDXGISwapChain *swapChain) {
  SKSE::log::info("Initializing ImGui...");

  // Get D3D11 device and context
  if (FAILED(swapChain->GetDevice(__uuidof(ID3D11Device),
                                  reinterpret_cast<void **>(&device)))) {
    SKSE::log::error("Failed to get D3D11 device from swap chain");
    return false;
  }
  device->GetImmediateContext(&context);

  // Get back buffer and create render target view
  ID3D11Texture2D *backBuffer = nullptr;
  if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                  reinterpret_cast<void **>(&backBuffer)))) {
    SKSE::log::error("Failed to get back buffer");
    return false;
  }

  if (FAILED(device->CreateRenderTargetView(backBuffer, nullptr,
                                            &renderTargetView))) {
    backBuffer->Release();
    SKSE::log::error("Failed to create render target view");
    return false;
  }
  backBuffer->Release();

  // Get the render window handle
  DXGI_SWAP_CHAIN_DESC desc;
  swapChain->GetDesc(&desc);
  HWND hwnd = desc.OutputWindow;

  // Initialize ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // Don't change cursor

  // Initialize platform/renderer backends
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(device, context);

  // Set up style
  ImGui::StyleColorsDark();

  imguiInitialized = true;
  SKSE::log::info("ImGui initialized successfully");

  // Initialize the overlay (loads fonts)
  Easy2Read::Overlay::GetSingleton()->Initialize();

  return true;
}

void D3D11Hook::RenderImGui() {
  if (!imguiInitialized) {
    return;
  }

  // Start new frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // Call the render callback to draw our overlay
  if (renderCallback) {
    renderCallback();
  }

  // Render
  ImGui::Render();
  context->OMSetRenderTargets(1, &renderTargetView, nullptr);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

} // namespace Easy2Read
