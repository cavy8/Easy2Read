#include "D3D11Hook.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "PCH.h"
#include "UI/Overlay.h"
#include <imgui.h>

namespace Easy2Read {
namespace {
class ImGuiContextScope {
public:
  explicit ImGuiContextScope(ImGuiContext *current)
      : previous(ImGui::GetCurrentContext()) {
    ImGui::SetCurrentContext(current);
  }
  ~ImGuiContextScope() { ImGui::SetCurrentContext(previous); }

private:
  ImGuiContext *previous;
};
} // namespace

D3D11Hook *D3D11Hook::GetSingleton() {
  static D3D11Hook singleton;
  return &singleton;
}

bool D3D11Hook::Install() {
  if (initialized) {
    return true;
  }

  // Draw after the book, while CS's UI buffer is still the framebuffer.
  // Present can target the scene buffer and runs too late to join the UI layer.
  REL::Relocation<std::uintptr_t> vtable(RE::VTABLE_BookMenu[0]);
  originalPostDisplay = vtable.write_vfunc(0x6, HookedPostDisplay);
  initialized = true;
  SKSE::log::info("D3D11 hook installed at BookMenu::PostDisplay");
  return true;
}

void D3D11Hook::Uninstall() {
  if (initialized) {
    REL::Relocation<std::uintptr_t> vtable(RE::VTABLE_BookMenu[0]);
    auto current = *reinterpret_cast<std::uintptr_t *>(
        vtable.address() + 0x6 * sizeof(void *));
    if (current == reinterpret_cast<std::uintptr_t>(&HookedPostDisplay)) {
      vtable.write_vfunc(0x6, originalPostDisplay.address());
    }
    initialized = false;
  }

  if (imguiInitialized) {
    if (ImGui::GetCurrentContext() == imguiContext) {
      ImGui::SetCurrentContext(nullptr);
    }
    ImGuiContextScope scope(imguiContext);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(imguiContext);
    imguiContext = nullptr;
    imguiInitialized = false;
  }
  if (context) {
    context->Release();
    context = nullptr;
  }
  if (device) {
    device->Release();
    device = nullptr;
  }
  SKSE::log::info("D3D11 hook uninstalled");
}

void D3D11Hook::SetRenderCallback(RenderCallback callback) {
  renderCallback = std::move(callback);
}

void D3D11Hook::HookedPostDisplay(RE::BookMenu *menu) {
  originalPostDisplay(menu);
  auto hook = GetSingleton();
  if (!hook->initialized) {
    return;
  }
  if (!hook->imguiInitialized && !hook->InitImGui()) {
    return;
  }
  hook->RenderImGui();
}

bool D3D11Hook::InitImGui() {
  auto renderer = RE::BSGraphics::Renderer::GetSingleton();
  if (!renderer) {
    return false;
  }
  auto &renderData = renderer->GetRuntimeData();
  auto renderDevice = reinterpret_cast<ID3D11Device *>(renderData.forwarder);
  auto hwnd = reinterpret_cast<HWND>(renderData.renderWindows[0].hWnd);
  if (!renderDevice || !hwnd) {
    SKSE::log::error("Cannot initialize ImGui: renderer device/window unavailable");
    return false;
  }

  device = renderDevice;
  device->AddRef();
  device->GetImmediateContext(&context);

  IMGUI_CHECKVERSION();
  ImGuiContextScope scope(ImGui::GetCurrentContext());
  imguiContext = ImGui::CreateContext();
  ImGui::SetCurrentContext(imguiContext);
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

  bool platformReady = ImGui_ImplWin32_Init(hwnd);
  bool rendererReady = platformReady && ImGui_ImplDX11_Init(device, context);
  if (!rendererReady) {
    if (platformReady) {
      ImGui_ImplWin32_Shutdown();
    }
    ImGui::DestroyContext(imguiContext);
    imguiContext = nullptr;
    context->Release();
    context = nullptr;
    device->Release();
    device = nullptr;
    SKSE::log::error("Failed to initialize ImGui backends");
    return false;
  }

  ImGui::StyleColorsDark();
  Overlay::GetSingleton()->Initialize();
  imguiInitialized = true;
  SKSE::log::info("ImGui initialized for book UI rendering");
  return true;
}

void D3D11Hook::RenderImGui() {
  auto renderer = RE::BSGraphics::Renderer::GetSingleton();
  if (!imguiInitialized || !renderer) {
    return;
  }

  // Read this every draw: CS redirects it for FG/HDR and menu transitions.
  // Neither GetDevice success nor the currently bound RTV identifies that path.
  auto renderTarget = reinterpret_cast<ID3D11RenderTargetView *>(
      renderer->GetRuntimeData().renderTargets[RE::RENDER_TARGET::kFRAMEBUFFER].RTV);
  if (!renderTarget) {
    return;
  }

  ImGuiContextScope scope(imguiContext);
  Overlay::GetSingleton()->RefreshResources();
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  if (renderCallback) {
    renderCallback();
  }
  ImGui::Render();
  if (ImGui::GetDrawData()->TotalVtxCount == 0) {
    return;
  }

  // The DX11 backend restores pipeline state, but not render target bindings.
  ID3D11RenderTargetView *savedTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
  ID3D11DepthStencilView *savedDepth = nullptr;
  context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
                              savedTargets, &savedDepth);
  context->OMSetRenderTargets(1, &renderTarget, nullptr);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
                              savedTargets, savedDepth);
  for (auto target : savedTargets) {
    if (target) {
      target->Release();
    }
  }
  if (savedDepth) {
    savedDepth->Release();
  }
}

} // namespace Easy2Read
