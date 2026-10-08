#pragma once

#include "BookPromptIcon.h"
#include <imgui.h>
#include <string>
#include <atomic>

namespace Easy2Read {

class Overlay {
public:
  [[nodiscard]] static Overlay *GetSingleton();

  // Initialize the overlay (load fonts, etc.)
  void Initialize();
  void RequestResourceRefresh(bool fonts = true) {
    if (fonts) fontRefreshPending.store(true);
    resourceRefreshPending.store(true);
  }
  void RefreshResources(); // Called with our context current, before NewFrame.

  // Main render function - called every frame by D3D11Hook
  void Render();

  // Set content to display
  void SetContent(const std::string &title, const std::string &text);
  void ClearContent();

  // Visibility control
  void Show();
  void Hide();
  void Toggle();
  [[nodiscard]] bool IsVisible() const { return visible; }

  // Scroll input - accumulates scroll delta from InputHandler
  void AddScrollInput(float delta);

private:
  Overlay() = default;
  Overlay(const Overlay &) = delete;
  Overlay(Overlay &&) = delete;
  ~Overlay() = default;

  Overlay &operator=(const Overlay &) = delete;
  Overlay &operator=(Overlay &&) = delete;

  void RenderWindow();
  void RenderBookPrompt();
  void LoadFont();

  bool visible = false;
  bool fontLoaded = false;
  std::atomic<bool> resourceRefreshPending{false};
  std::atomic<bool> fontRefreshPending{false};

  std::string bookTitle;
  std::string bookText;

  // Accumulated scroll input from InputHandler
  float pendingScrollDelta = 0.0f;

  // Reset scroll position on next frame
  bool resetScrollOnNextFrame = false;

  // ImGui font pointer
  ImFont *customFont = nullptr;
  int skyUIFrameRect = -1; // Extracted frame rectangle in the font atlas
  BookPromptIcon keyboardPromptIcon;
  BookPromptIcon controllerPromptIcon;
};

} // namespace Easy2Read
