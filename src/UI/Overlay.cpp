#include "Overlay.h"
#include "SkyUIFrame.h"
#include "Config/Settings.h"
#include "ImGui/imgui_impl_dx11.h"
#include "OverlayControl.h"
#include "Hooks/MenuWatcher.h"
#include "PCH.h"
#include <limits>
#include <imgui.h>

namespace Easy2Read {
void RequestOverlayResourceRefresh(bool fonts) {
  Overlay::GetSingleton()->RequestResourceRefresh(fonts);
}
void HideReadingOverlay() { Overlay::GetSingleton()->Hide(); }
namespace {
std::string GetPromptButton(bool gamepad, const Settings &settings) {
  if (gamepad) {
    switch (settings.controllerToggleButton) {
    case 0x0001: return "D-Pad Up";
    case 0x0002: return "D-Pad Down";
    case 0x0004: return "D-Pad Left";
    case 0x0008: return "D-Pad Right";
    case 0x0010: return "Start";
    case 0x0020: return "Back";
    case 0x0040: return "LS";
    case 0x0080: return "RS";
    case 0x0100: return "LB";
    case 0x0200: return "RB";
    case 0x1000: return "A";
    case 0x2000: return "B";
    case 0x4000: return "X";
    case 0x8000: return "Y";
    default: return fmt::format("0x{:X}", settings.controllerToggleButton);
    }
  }
  RE::BSFixedString name;
  auto input = RE::BSInputDeviceManager::GetSingleton();
  if (input && input->GetButtonNameFromID(RE::INPUT_DEVICE::kKeyboard,
                                        settings.toggleKey, name) &&
      !name.empty()) {
    return name.c_str();
  }
  return fmt::format("Key {}", settings.toggleKey);
}
} // namespace

Overlay *Overlay::GetSingleton() {
  static Overlay singleton;
  return &singleton;
}

void Overlay::Initialize() {
  SKSE::log::info("Initializing Overlay...");
  LoadFont();
  auto settings = Settings::GetSingleton();
  if (settings->overlayEnabled && settings->showBookPrompt) {
    keyboardPromptIcon.Load(settings->toggleKey, false);
    controllerPromptIcon.Load(settings->controllerToggleButton, true);
  }
}

void Overlay::LoadFont() {
  auto settings = Settings::GetSingleton();

  ImGuiIO &io = ImGui::GetIO();

  // Build glyph ranges based on configured language support level
  // Using static to cache the built ranges (rebuilt if setting changes)
  static ImVector<ImWchar> glyphRanges;
  static LanguageSupport lastLanguageSupport = LanguageSupport::Latin;

  if (glyphRanges.empty() || lastLanguageSupport != settings->languageSupport) {
    glyphRanges.clear();
    lastLanguageSupport = settings->languageSupport;

    ImFontGlyphRangesBuilder builder;

    // Always include basic Latin
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());

    // Additional ranges based on language support level
    switch (settings->languageSupport) {
    case LanguageSupport::Full:
      // Full: Everything including full CJK
      builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
      builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
      builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
      builder.AddRanges(io.Fonts->GetGlyphRangesThai());
      builder.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
      [[fallthrough]];

    case LanguageSupport::Asian:
      // Asian: Common CJK characters (smaller than full)
      if (settings->languageSupport == LanguageSupport::Asian) {
        builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
        builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
      }
      [[fallthrough]];

    case LanguageSupport::European:
      // European: Cyrillic, Greek, extended Latin
      builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
      builder.AddRanges(io.Fonts->GetGlyphRangesGreek());
      {
        // Extended Latin ranges for European languages
        static const ImWchar europeanRanges[] = {
            0x0100, 0x017F, // Latin Extended-A (Central/Eastern European)
            0x0180, 0x024F, // Latin Extended-B
            0x1E00, 0x1EFF, // Latin Extended Additional
            0,
        };
        builder.AddRanges(europeanRanges);
      }
      [[fallthrough]];

    case LanguageSupport::Latin:
    default:
      // Latin: Just basic punctuation and symbols
      {
        static const ImWchar latinRanges[] = {
            0x2000, 0x206F, // General Punctuation
            0x20A0, 0x20CF, // Currency Symbols
            0,
        };
        builder.AddRanges(latinRanges);
      }
      break;
    }

    builder.BuildRanges(&glyphRanges);

    const char *levelName = "Latin";
    if (settings->languageSupport == LanguageSupport::European)
      levelName = "European";
    else if (settings->languageSupport == LanguageSupport::Asian)
      levelName = "Asian";
    else if (settings->languageSupport == LanguageSupport::Full)
      levelName = "Full";

    SKSE::log::info("Built {} Unicode glyph ranges for font loading",
                    levelName);
  }

  // Try to load the configured font
  std::string fontPath = settings->GetFontPath();

  // Only try to load if path is not empty (empty = use default)
  if (!fontPath.empty() && std::filesystem::exists(fontPath)) {
    ImFontConfig config;
    config.GlyphRanges = glyphRanges.Data;
    customFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(),
                                              settings->fontSize, &config);

    if (customFont) {
      SKSE::log::info(
          "Loaded custom font with full Unicode support: {} (size: {})",
          fontPath, settings->fontSize);
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
    // Note: Default font only supports basic Latin - use a custom font for full
    // Unicode
    ImFontConfig config;
    config.SizePixels = settings->fontSize;
    customFont = io.Fonts->AddFontDefault(&config);
  }

  // Include the extracted frame in the same texture as the fonts.
  skyUIFrameRect = SkyUIFrame::AddToAtlas(*io.Fonts);
  if (skyUIFrameRect < 0) {
    SKSE::log::warn("Could not add SkyUI frame artwork to font atlas; using plain border");
  }
}

void Overlay::RefreshResources() {
  if (!resourceRefreshPending.exchange(false)) {
    return;
  }
  if (fontRefreshPending.exchange(false)) {
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui::GetIO().Fonts->Clear();
    customFont = nullptr;
    fontLoaded = false;
    skyUIFrameRect = -1;
    LoadFont();
  }
  keyboardPromptIcon = BookPromptIcon{};
  controllerPromptIcon = BookPromptIcon{};
  const auto settings = Settings::GetSingleton();
  if (settings->overlayEnabled && settings->showBookPrompt) {
    keyboardPromptIcon.Load(settings->toggleKey, false);
    controllerPromptIcon.Load(settings->controllerToggleButton, true);
  }
}

void Overlay::Render() {
  auto settings = Settings::GetSingleton();
  if (!settings->overlayEnabled ||
      !MenuWatcher::GetSingleton()->IsBookMenuOpen()) {
    return;
  }
  if (!visible) {
    if (settings->showBookPrompt) {
      RenderBookPrompt();
    }
    return;
  }

  RenderWindow();
}

void Overlay::RenderBookPrompt() {
  auto settings = Settings::GetSingleton();
  auto ui = RE::UI::GetSingleton();
  auto menu = ui ? ui->GetMenu<RE::BookMenu>() : nullptr;
  if (!menu || !menu->GetRuntimeData().bookInitialized) {
    return;
  }

  const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
  if (displaySize.x <= 0.0f || displaySize.y <= 0.0f) {
    return;
  }
  const float scale = displaySize.y / 1080.0f;
  const float fontSize = 28.0f * scale;
  auto font = customFont ? customFont : ImGui::GetFont();
  auto input = RE::BSInputDeviceManager::GetSingleton();
  const bool gamepad = input && input->IsGamepadEnabled();
  if ((gamepad ? settings->controllerToggleButton : settings->toggleKey) == 0) {
    return;
  }
  const auto &icon = gamepad ? controllerPromptIcon : keyboardPromptIcon;
  const std::string button = GetPromptButton(gamepad, *settings);
  constexpr auto label = "Show Text";
  const ImVec2 buttonTextSize = font->CalcTextSizeA(
      fontSize, (std::numeric_limits<float>::max)(), 0.0f, button.c_str());
  const ImVec2 labelSize = font->CalcTextSizeA(
      fontSize, (std::numeric_limits<float>::max)(), 0.0f, label);
  const float badgeHeight = fontSize + 8.0f * scale;
  const float badgeWidth = icon.GetTexture()
                               ? badgeHeight * icon.GetAspectRatio()
                               : (std::max)(badgeHeight, buttonTextSize.x + 16.0f * scale);
  const float gap = 10.0f * scale;
  const float width = badgeWidth + gap + labelSize.x;

  // Reserve the bottom strip for the game's Take/Exit/page controls.
  const ImVec2 position((displaySize.x - width) * 0.5f,
                        displaySize.y * 0.90f - badgeHeight);

  auto draw = ImGui::GetForegroundDrawList();
  const ImVec2 badgeEnd(position.x + badgeWidth, position.y + badgeHeight);
  const float rounding = gamepad && button.size() == 1
                             ? badgeHeight * 0.5f : 2.0f * scale;
  const ImVec2 buttonPos(position.x + (badgeWidth - buttonTextSize.x) * 0.5f,
                         position.y + (badgeHeight - buttonTextSize.y) * 0.5f);
  if (icon.GetTexture()) {
    draw->AddImage(reinterpret_cast<ImTextureID>(icon.GetTexture()), position, badgeEnd);
  } else {
    draw->AddRectFilled(position, badgeEnd, IM_COL32(0, 0, 0, 150), rounding);
    draw->AddRect(position, badgeEnd, IM_COL32(255, 255, 255, 230), rounding,
                  0, scale);
    draw->AddText(font, fontSize, buttonPos, IM_COL32_WHITE, button.c_str());
  }
  const ImVec2 labelPos(badgeEnd.x + gap,
                        position.y + (badgeHeight - labelSize.y) * 0.5f);
  draw->AddText(font, fontSize, ImVec2(labelPos.x + scale, labelPos.y + scale),
                IM_COL32(0, 0, 0, 230), label);
  draw->AddText(font, fontSize, labelPos, IM_COL32_WHITE, label);
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
  const bool useSkyUIFrame = settings->showBorder &&
                            settings->showCornerOrnaments &&
                            settings->borderSize > 0.0f && skyUIFrameRect >= 0;
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,
                      settings->showBorder && !useSkyUIFrame
                          ? settings->borderSize : 0.0f);
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
    if (useSkyUIFrame) {
      SkyUIFrame::Draw(*ImGui::GetWindowDrawList(), *io.Fonts, skyUIFrameRect,
                       ImGui::GetWindowPos(), ImGui::GetWindowSize(),
                       ImGui::ColorConvertFloat4ToU32(borderColor),
                       io.DisplaySize.y / 1080.0f * settings->borderSize);
    }

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
      if (settings->centerTitle) {
        const float width = (std::max)(1.0f, ImGui::GetContentRegionAvail().x);
        const ImVec2 titleSize =
            ImGui::CalcTextSize(bookTitle.c_str(), nullptr, false, width);
        ImVec2 position = ImGui::GetCursorScreenPos();
        position.x += (std::max)(0.0f, (width - titleSize.x) * 0.5f);
        ImGui::GetWindowDrawList()->AddText(
            ImGui::GetFont(), ImGui::GetFontSize(), position,
            ImGui::ColorConvertFloat4ToU32(titleColor), bookTitle.c_str(),
            nullptr, width);
        ImGui::Dummy(ImVec2(width, titleSize.y));
      } else {
        ImGui::TextWrapped("%s", bookTitle.c_str());
      }
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

    // Reset scroll to top when overlay was just opened
    if (resetScrollOnNextFrame) {
      ImGui::SetScrollY(0.0f);
      resetScrollOnNextFrame = false;
    }

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

void Overlay::Show(bool useMouse) {
  if (!visible) {
    visible = true;
    mouseEnabled = useMouse;
    resetScrollOnNextFrame = true;
    centerMouseOnNextFrame = true;
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

void Overlay::AddMouseMove(float dx, float dy) {
  if (dx != 0.0f || dy != 0.0f) {
    mouseEnabled = true;
  }
  pendingMouseDelta.x += dx;
  pendingMouseDelta.y += dy;
}

void Overlay::QueueMouseButton(std::uint32_t button, bool down) {
  if (button < ImGuiMouseButton_COUNT) {
    if (down) {
      mouseEnabled = true;
    }
    pendingMouseButtons.emplace_back(static_cast<int>(button), down);
  }
}

void Overlay::UpdateMouse() {
  ImGuiIO &io = ImGui::GetIO();
  io.MouseDrawCursor = visible && mouseEnabled;
  if (!visible || !mouseEnabled) {
    pendingMouseDelta = ImVec2(0.0f, 0.0f);
    pendingMouseButtons.clear();
    // Release anything held when the overlay closed; ImGui drops repeats.
    for (int button = 0; button < ImGuiMouseButton_COUNT; ++button) {
      io.AddMouseButtonEvent(button, false);
    }
    io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    return;
  }

  if (centerMouseOnNextFrame) {
    mousePos = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    centerMouseOnNextFrame = false;
  }
  mousePos.x = std::clamp(mousePos.x + pendingMouseDelta.x, 0.0f,
                          (std::max)(0.0f, io.DisplaySize.x - 1.0f));
  mousePos.y = std::clamp(mousePos.y + pendingMouseDelta.y, 0.0f,
                          (std::max)(0.0f, io.DisplaySize.y - 1.0f));
  pendingMouseDelta = ImVec2(0.0f, 0.0f);

  // Queued after the Win32 backend's cursor fallback, so this position wins.
  io.AddMousePosEvent(mousePos.x, mousePos.y);
  for (const auto &[button, down] : pendingMouseButtons) {
    io.AddMouseButtonEvent(button, down);
  }
  pendingMouseButtons.clear();
}

} // namespace Easy2Read
