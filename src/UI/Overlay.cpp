#include "Overlay.h"
#include "Config/Settings.h"
#include "PCH.h"
#include <imgui.h>

namespace Easy2Read {

Overlay *Overlay::GetSingleton() {
  static Overlay singleton;
  return &singleton;
}

void Overlay::Initialize() {
  SKSE::log::info("Initializing Overlay...");
  LoadFont();
}

void Overlay::LoadFont() {
  auto settings = Settings::GetSingleton();

  ImGuiIO &io = ImGui::GetIO();

  // Try to load the configured font
  std::string fontPath = settings->GetFontPath();

  // Only try to load if path is not empty (empty = use default)
  if (!fontPath.empty() && std::filesystem::exists(fontPath)) {
    customFont =
        io.Fonts->AddFontFromFileTTF(fontPath.c_str(), settings->fontSize);

    if (customFont) {
      SKSE::log::info("Loaded custom font: {} (size: {})", fontPath,
                      settings->fontSize);
      fontLoaded = true;
    } else {
      SKSE::log::warn("Failed to load font: {}", fontPath);
    }
  } else if (!fontPath.empty()) {
    SKSE::log::warn("Font file not found: {}", fontPath);
  }

  // Fall back to default if no custom font loaded
  if (!customFont) {
    SKSE::log::info("Using ImGui default font (size: {})", settings->fontSize);
    // Load default font at configured size
    ImFontConfig config;
    config.SizePixels = settings->fontSize;
    customFont = io.Fonts->AddFontDefault(&config);
  }

  // Build fonts
  io.Fonts->Build();
}

void Overlay::Render() {
  if (!visible) {
    return;
  }

  RenderWindow();
}

void Overlay::RenderWindow() {
  auto settings = Settings::GetSingleton();

  // Get display size for percentage-based sizing
  ImGuiIO &io = ImGui::GetIO();

  // Calculate window size as percentage of screen
  float windowWidth =
      io.DisplaySize.x * (settings->windowWidthPercent / 100.0f);
  float windowHeight =
      io.DisplaySize.y * (settings->windowHeightPercent / 100.0f);

  // Center the window
  ImVec2 windowPos((io.DisplaySize.x - windowWidth) / 2.0f,
                   (io.DisplaySize.y - windowHeight) / 2.0f);

  // Always set position and size (no user resizing)
  ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);

  // Configure all style settings from theme
  float opacity = settings->windowAlpha;

  // Window colors
  ImVec4 bgColor(settings->windowColorR / 255.0f,
                 settings->windowColorG / 255.0f,
                 settings->windowColorB / 255.0f, opacity);
  ImVec4 titleBgColor(settings->windowColorR / 255.0f * 0.8f,
                      settings->windowColorG / 255.0f * 0.8f,
                      settings->windowColorB / 255.0f * 0.8f, opacity);

  // Border color (with per-element alpha)
  ImVec4 borderColor(settings->borderColorR / 255.0f,
                     settings->borderColorG / 255.0f,
                     settings->borderColorB / 255.0f,
                     settings->showBorder ? settings->borderAlpha : 0.0f);

  // Separator color (with per-element alpha)
  ImVec4 separatorColor(
      settings->separatorColorR / 255.0f, settings->separatorColorG / 255.0f,
      settings->separatorColorB / 255.0f, settings->separatorAlpha);

  // Scrollbar colors (with per-element alpha)
  ImVec4 scrollbarBgColor(
      settings->scrollbarBgColorR / 255.0f,
      settings->scrollbarBgColorG / 255.0f,
      settings->scrollbarBgColorB / 255.0f,
      settings->showScrollbarTrack ? settings->scrollbarTrackAlpha : 0.0f);
  ImVec4 scrollbarColor(
      settings->scrollbarColorR / 255.0f, settings->scrollbarColorG / 255.0f,
      settings->scrollbarColorB / 255.0f, settings->scrollbarThumbAlpha);
  ImVec4 scrollbarHoverColor(settings->scrollbarHoverColorR / 255.0f,
                             settings->scrollbarHoverColorG / 255.0f,
                             settings->scrollbarHoverColorB / 255.0f,
                             settings->scrollbarThumbAlpha);

  // Push all colors
  ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
  ImGui::PushStyleColor(ImGuiCol_TitleBg, titleBgColor);
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, titleBgColor);
  ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
  ImGui::PushStyleColor(ImGuiCol_Separator, separatorColor);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, scrollbarBgColor);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, scrollbarColor);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, scrollbarHoverColor);
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, scrollbarHoverColor);

  // Push style vars (sizes, rounding, etc.)
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, settings->windowRounding);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                      ImVec2(settings->windowPadding, settings->windowPadding));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,
                      settings->showBorder ? settings->borderSize : 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, settings->scrollbarSize);
  ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding,
                      settings->scrollbarRounding);

  // Window flags - remove all interactivity except scrolling
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoTitleBar;

  // Push custom font
  if (customFont) {
    ImGui::PushFont(customFont);
  }

  if (ImGui::Begin("###Easy2ReadOverlay", nullptr, flags)) {
    // Title section (conditionally rendered)
    if (settings->showTitle) {
      ImVec4 titleColor(settings->titleColorR / 255.0f,
                        settings->titleColorG / 255.0f,
                        settings->titleColorB / 255.0f, 1.0f);

      // Draw title with scaled font
      ImGui::PushStyleColor(ImGuiCol_Text, titleColor);
      float originalScale = ImGui::GetFont()->Scale;
      ImGui::GetFont()->Scale *= settings->titleScale;
      ImGui::PushFont(ImGui::GetFont());
      ImGui::TextWrapped("%s", bookTitle.c_str());
      ImGui::PopFont();
      ImGui::GetFont()->Scale = originalScale;
      ImGui::PopStyleColor();

      // Separator under title (conditionally rendered)
      if (settings->showSeparator) {
        ImGui::Separator();
      }
      ImGui::Spacing();
    }

    // Body text color
    ImVec4 bodyColor(settings->bodyColorR / 255.0f,
                     settings->bodyColorG / 255.0f,
                     settings->bodyColorB / 255.0f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, bodyColor);

    // Scrollable text region
    ImGui::BeginChild("BookTextScroll", ImVec2(0, 0), false,
                      ImGuiWindowFlags_None);

    // Apply any pending scroll input directly (bypasses mouse position check)
    if (pendingScrollDelta != 0.0f) {
      float currentScroll = ImGui::GetScrollY();
      float newScroll =
          currentScroll - (pendingScrollDelta * settings->scrollSpeed);
      ImGui::SetScrollY(newScroll);
      pendingScrollDelta = 0.0f;
    }

    // Render book text with word wrapping
    ImGui::TextWrapped("%s", bookText.c_str());

    ImGui::EndChild();

    ImGui::PopStyleColor(); // Body text color
  }
  ImGui::End();

  // Pop custom font
  if (customFont) {
    ImGui::PopFont();
  }

  ImGui::PopStyleVar(5);   // Style vars (rounding, padding, border, scrollbar)
  ImGui::PopStyleColor(9); // All colors pushed above
}

void Overlay::SetContent(const std::string &title, const std::string &text) {
  bookTitle = title;
  bookText = text;
  SKSE::log::debug("Overlay content set: {}", title);
}

void Overlay::ClearContent() {
  bookTitle.clear();
  bookText.clear();
}

void Overlay::Show() {
  if (!visible) {
    visible = true;
    SKSE::log::info("Overlay shown");
  }
}

void Overlay::Hide() {
  if (visible) {
    visible = false;
    SKSE::log::info("Overlay hidden");
  }
}

void Overlay::Toggle() {
  if (visible) {
    Hide();
  } else {
    Show();
  }
}

void Overlay::AddScrollInput(float delta) { pendingScrollDelta += delta; }

} // namespace Easy2Read
