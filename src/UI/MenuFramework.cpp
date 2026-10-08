#include "PCH.h"
#include "MenuFramework.h"
#include "Config/SettingsSchema.h"
#include "OverlayControl.h"
#include "MenuBindings.h"
#include <cmath>
#include <cctype>
#include <type_traits>

namespace Easy2Read::MenuFramework {
namespace {
namespace UI = ImGuiMCP;
std::string generalStatus;
std::string themeStatus;

bool EditBinding(const SettingField &field, std::uint32_t &code) {
  const bool controller = std::string_view(field.key) == "ControllerToggleButton";
  auto key = ToPickerKey(code, controller);
  if (key == UI::ImGuiKey_None && code != 0) {
    UI::TextWrapped("This INI binding is not recognized by the picker. It is retained until you assign or clear it.");
    if (UI::Button(controller ? "Clear unrecognized controller binding" : "Clear unrecognized keyboard binding")) {
      code = 0;
      return true;
    }
  }
  if (!ImGuiMCPComponents::KeyBindingPicker(
          controller ? "Controller toggle button" : "Keyboard toggle key", &key)) {
    return false;
  }
  const auto converted = FromPickerKey(key, controller);
  if (!converted) {
    generalStatus = controller ? "Choose a controller button for this binding."
                               : "Choose a keyboard key for this binding.";
    return false;
  }
  code = *converted;
  return true;
}

bool EditFontFile(std::string &value) {
  static std::vector<std::string> fonts;
  static bool scanned = false;
  auto scan = [&]() {
    fonts.clear();
    for (const auto *directory : {"Data/SKSE/Plugins/Easy2Read", "Data/SKSE/Plugins/Fonts"}) {
      std::error_code error;
      std::filesystem::directory_iterator it(directory, error), end;
      while (!error && it != end) {
        auto extension = it->path().extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (it->is_regular_file(error) && (extension == ".ttf" || extension == ".otf")) {
          fonts.push_back(it->path().lexically_relative("Data").generic_string());
        }
        it.increment(error);
      }
    }
    std::sort(fonts.begin(), fonts.end());
    scanned = true;
  };
  if (!scanned) scan();
  bool changed = false;
  if (UI::BeginCombo("Installed font", value.c_str())) {
    for (const auto &font : fonts) {
      if (UI::Selectable(font.c_str(), font == value)) {
        value = font;
        changed = true;
      }
    }
    UI::EndCombo();
  }
  UI::SameLine();
  if (UI::Button("Refresh font list")) scan();
  std::vector<char> buffer((std::max)(value.size() + 1, std::size_t{4096}), '\0');
  std::copy(value.begin(), value.end(), buffer.begin());
  if (UI::InputText("Font path (relative to Data)", buffer.data(), buffer.size())) {
    value = buffer.data();
    changed = true;
  }
  return changed;
}

bool Edit(const SettingField &field, Settings &settings) {
  return std::visit([&](auto member) -> bool {
    auto &value = settings.*member;
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, bool>) {
      return UI::Checkbox(field.label, &value);
    } else if constexpr (std::is_same_v<T, std::string>) {
      if (settings.fontPreset != FontPreset::Custom) return false;
      return EditFontFile(value);
    } else if constexpr (std::is_same_v<T, FontPreset> || std::is_same_v<T, LanguageSupport>) {
      int selected = static_cast<int>(value);
      const char *const *names;
      int count;
      if constexpr (std::is_same_v<T, FontPreset>) {
        names = fontPresetNames;
        count = static_cast<int>(std::size(fontPresetNames));
      } else {
        names = languageSupportNames;
        count = static_cast<int>(std::size(languageSupportNames));
      }
      if (UI::Combo(field.label, &selected, names, count)) {
        value = static_cast<T>(selected);
        return true;
      }
      return false;
    } else if constexpr (std::is_same_v<T, float>) {
      const auto key = std::string_view(field.key);
      const bool opacity = std::string_view(field.section) == "Transparency";
      const bool titleScale = key == "TitleScale";
      const float scale = opacity || titleScale ? 100.0f : 1.0f;
      const char *format = "%.1f";
      if (opacity || titleScale || key == "WidthPercent" || key == "HeightPercent") format = "%.0f%%";
      else if (key == "BorderSize") format = "%.2fx";
      else if (key == "ScrollSpeed") format = "%.0f px/tick";
      else if (key != "ControllerScrollSpeed") format = "%.0f px";
      float edited = value * scale;
      if (UI::SliderFloat(field.label, &edited, field.minimum * scale,
                          field.maximum * scale, format,
                          UI::ImGuiSliderFlags_AlwaysClamp)) {
        value = (opacity ? std::round(edited) : edited) / scale;
        return true;
      }
      return false;
    } else if constexpr (std::is_same_v<T, std::uint32_t>) {
      return EditBinding(field, value);
    } else {
      for (const auto &color : colorSettings) {
        if (member != color.channels[0]) continue;
        float rgb[3];
        for (int i = 0; i < 3; ++i) rgb[i] = (settings.*color.channels[i]) / 255.0f;
        if (UI::ColorEdit3(color.label, rgb, UI::ImGuiColorEditFlags_Uint8)) {
          for (int i = 0; i < 3; ++i) {
            settings.*color.channels[i] = static_cast<std::uint8_t>(
                std::lround(std::clamp(rgb[i], 0.0f, 1.0f) * 255.0f));
          }
          return true;
        }
      }
      return false;
    }
  }, field.member);
}

bool IsRelevant(const SettingField &field, const Settings &settings) {
  const auto key = std::string_view(field.key);
  if (key.starts_with("TitleColor") || key == "TitleScale" || key == "CenterTitle") return settings.showTitle;
  if (key.starts_with("BorderColor") || key == "BorderSize" || key == "BorderAlpha" || key == "ShowCornerOrnaments") return settings.showBorder;
  if (key.starts_with("SeparatorColor") || key == "SeparatorAlpha") return settings.showSeparator;
  if (key.starts_with("BackgroundColor") || key == "ScrollbarTrackAlpha") return settings.showScrollbarTrack;
  return true;
}

void Render(bool general) {
  auto &settings = *Settings::GetSingleton();
  auto &status = general ? generalStatus : themeStatus;
  UI::TextWrapped("%s", general ? "Easy2Read.ini" : "Easy2Read_Theme.ini");
  UI::TextWrapped("Changes apply while playing. Save writes this page to its INI; Reload discards unsaved changes on this page.");
  if (UI::Button("Save INI")) {
    const bool saved = general ? settings.SaveGeneral() : settings.SaveTheme();
    status = saved ? "Saved." : "Save failed. Check Easy2Read.log and file permissions.";
  }
  UI::SameLine();
  if (UI::Button("Reload INI")) {
    const bool loaded = general ? settings.LoadGeneral() : settings.LoadTheme();
    if (loaded) {
      RequestOverlayResourceRefresh(!general);
    }
    if (!settings.overlayEnabled) {
      HideReadingOverlay();
    }
    status = loaded ? "Reloaded." : "Reload failed. Current settings kept; check Easy2Read.log.";
  }
  if (!status.empty()) {
    UI::TextWrapped("%s", status.c_str());
  }
  UI::Separator();
  if (general) {
    UI::TextWrapped("Click a binding and press the keyboard key or controller button you want to assign. Escape cancels; Clear disables that binding. Default: F on keyboard and Y on controller.");
  } else {
    UI::TextWrapped("Click a color swatch to open its picker. Opacity: 0%% is transparent, 100%% is opaque. Custom fonts can be selected from installed files or entered as a path relative to Data. Font changes refresh on the next book frame. Higher character coverage uses more memory.");
  }
  std::string_view section;
  bool expanded = true;
  for (const auto &field : settingFields) {
    if (field.IsGeneral() != general) {
      continue;
    }
    if (section != field.section) {
      section = field.section;
      expanded = general || UI::CollapsingHeader(field.section, UI::ImGuiTreeNodeFlags_DefaultOpen);
    }
    UI::PushID(field.section);
    UI::BeginDisabled(!IsRelevant(field, settings));
    if (expanded && Edit(field, settings)) {
      status = "Unsaved changes.";
      if (field.refreshResources) {
        RequestOverlayResourceRefresh(!general);
      }
      if (!settings.overlayEnabled) {
        HideReadingOverlay();
      }
    }
    UI::EndDisabled();
    UI::PopID();
  }
}

void __stdcall RenderGeneral() { Render(true); }
void __stdcall RenderTheme() { Render(false); }
} // namespace

void Register() {
  static bool registered = false;
  if (registered) {
    return;
  }
  // IsInstalled checks the file; verify the loaded module and our exports too.
  if (!SKSEMenuFramework::IsInstalled() || !GetMenuFrameworkModule()) {
    logger::info("SKSE Menu Framework not loaded; using INI configuration");
    return;
  }
  constexpr const char *required[] = {
      "AddSectionItem", "igTextWrappedV", "igButton", "igSameLine",
      "igSeparator", "igCheckbox", "igInputText", "igCombo_Str_arr",
      "igSliderFloat", "igColorEdit3", "igCollapsingHeader_TreeNodeFlags",
      "igPushID_Str", "igPopID", "igGetID_Str", "igGetKeyName",
      "igIsKeyPressed_Bool", "igSetNextFrameWantCaptureKeyboard",
      "igTextUnformatted", "igTextV", "igBeginCombo", "igEndCombo",
      "igSelectable_Bool", "igBeginDisabled", "igEndDisabled"};
  for (const auto *name : required) {
    if (!GetProcAddress(GetMenuFrameworkModule(), name)) {
      logger::warn("SKSE Menu Framework integration unavailable: missing {}", name);
      return;
    }
  }
  SKSEMenuFramework::SetSection("Easy2Read");
  SKSEMenuFramework::AddSectionItem("General", RenderGeneral);
  SKSEMenuFramework::AddSectionItem("Theme", RenderTheme);
  registered = true;
  logger::info("Registered Easy2Read General and Theme pages with SKSE Menu Framework");
}

bool IsInputCaptured() {
  using Function = bool (*)();
  static const auto function = GetMenuFrameworkFunction<Function>("IsAnyBlockingWindowOpened");
  return function && function();
}
} // namespace Easy2Read::MenuFramework
