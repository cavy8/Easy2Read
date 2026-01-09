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

  // Get display size for centering
  ImGuiIO &io = ImGui::GetIO();
  float windowWidth = settings->windowWidth;
  float windowHeight = settings->windowHeight;

  // Center the window
  ImVec2 windowPos((io.DisplaySize.x - windowWidth) / 2.0f,
                   (io.DisplaySize.y - windowHeight) / 2.0f);

  // Always set position and size (no user resizing)
  ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);

  // Configure window colors from theme
  float opacity = settings->windowOpacity;
  ImVec4 bgColor(settings->windowColorR / 255.0f,
                 settings->windowColorG / 255.0f,
                 settings->windowColorB / 255.0f, opacity);
  ImVec4 titleBgColor(settings->windowColorR / 255.0f * 0.8f,
                      settings->windowColorG / 255.0f * 0.8f,
                      settings->windowColorB / 255.0f * 0.8f, opacity);

  ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
  ImGui::PushStyleColor(ImGuiCol_TitleBg, titleBgColor);
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, titleBgColor);

  // Window flags - remove all interactivity
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoTitleBar;

  // Push custom font
  if (customFont) {
    ImGui::PushFont(customFont);
  }

  if (ImGui::Begin("###Easy2ReadOverlay", nullptr, flags)) {
    // Title with custom color
    ImVec4 titleColor(settings->titleColorR / 255.0f,
                      settings->titleColorG / 255.0f,
                      settings->titleColorB / 255.0f, 1.0f);

    // Draw title
    std::string displayTitle = isNote ? "📜 " : "📖 ";
    displayTitle += bookTitle;

    ImGui::PushStyleColor(ImGuiCol_Text, titleColor);
    ImGui::TextWrapped("%s", displayTitle.c_str());
    ImGui::PopStyleColor();

    // Separator under title
    ImGui::Separator();
    ImGui::Spacing();

    // Body text color
    ImVec4 bodyColor(settings->bodyColorR / 255.0f,
                     settings->bodyColorG / 255.0f,
                     settings->bodyColorB / 255.0f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, bodyColor);

    // Scrollable text region
    ImGui::BeginChild("BookTextScroll", ImVec2(0, 0), false,
                      ImGuiWindowFlags_None);

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

  ImGui::PopStyleColor(3); // Window colors
}

void Overlay::SetContent(const std::string &title, const std::string &text,
                         bool note) {
  bookTitle = title;
  bookText = text;
  isNote = note;
  SKSE::log::debug("Overlay content set: {}", title);
}

void Overlay::ClearContent() {
  bookTitle.clear();
  bookText.clear();
  isNote = false;
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

} // namespace Easy2Read
