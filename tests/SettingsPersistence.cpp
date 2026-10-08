#include "PCH.h"
#include "Config/SettingsSchema.h"
#include "UI/MenuBindings.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>

namespace {
void Check(bool condition, const char *message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}
std::string Read(const std::filesystem::path &path) {
  std::ifstream stream(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}
void CheckCoverage(const std::filesystem::path &path, bool general) {
  CSimpleIniA ini;
  ini.SetUnicode();
  Check(ini.LoadFile(path.c_str()) >= 0, "Cannot load fixture");
  CSimpleIniA::TNamesDepend sections;
  ini.GetAllSections(sections);
  std::size_t count = 0;
  for (const auto &section : sections) {
    CSimpleIniA::TNamesDepend keys;
    ini.GetAllKeys(section.pItem, keys);
    for (const auto &key : keys) {
      ++count;
      int matches = 0;
      for (const auto &field : Easy2Read::settingFields) {
        matches += field.IsGeneral() == general &&
                   std::string_view(field.section) == section.pItem &&
                   std::string_view(field.key) == key.pItem;
      }
      Check(matches == 1, "INI key missing or duplicated in menu/save schema");
    }
  }
  const auto expected = std::count_if(std::begin(Easy2Read::settingFields),
      std::end(Easy2Read::settingFields), [general](const auto &f) { return f.IsGeneral() == general; });
  Check(count == static_cast<std::size_t>(expected), "Schema has unshipped keys");
}
} // namespace

int main(int argc, char **argv) {
  try {
    Check(argc == 3, "Usage: SettingsPersistence <repo> <isolated working directory>");
    const auto repo = std::filesystem::absolute(argv[1]);
    const auto working = std::filesystem::absolute(argv[2]);
    std::filesystem::create_directories(working / "Data/SKSE/Plugins");
    std::filesystem::current_path(working);
    const auto general = std::filesystem::path("Data/SKSE/Plugins/Easy2Read.ini");
    const auto theme = std::filesystem::path("Data/SKSE/Plugins/Easy2Read_Theme.ini");
    const auto themes = std::filesystem::path("Data/SKSE/Plugins/Easy2Read/Themes");
    Check(!std::filesystem::exists(repo / theme), "User theme INI must not ship; it would overwrite user edits");
    CheckCoverage(repo / general, true);
    std::size_t presetCount = 0;
    for (const auto &preset : std::filesystem::directory_iterator(repo / themes)) {
      CheckCoverage(preset.path(), false);
      ++presetCount;
    }
    std::filesystem::remove_all(themes);
    std::filesystem::create_directories(themes.parent_path());
    std::filesystem::copy(repo / themes, themes, std::filesystem::copy_options::recursive);
    std::filesystem::copy_file(repo / general, general, std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file(themes / "Default.ini", theme, std::filesystem::copy_options::overwrite_existing);
    { std::ofstream out(general, std::ios::app); out << "\n; Keep this comment\n[UserExtension]\nCustom = keep-me\n"; }
    { std::ofstream out(theme, std::ios::app); out << "\n; Keep theme comment\n[UserExtension]\nCustom = keep-theme\n"; }
    auto &settings = *Easy2Read::Settings::GetSingleton();
    settings.Load();
    std::vector<bool> expectedBools;
    for (const auto &field : Easy2Read::settingFields) {
      std::visit([&](auto member) {
        auto &value = settings.*member;
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, bool>) { value = !value; expectedBools.push_back(value); }
        else if constexpr (std::is_same_v<T, std::string>) value = "SKSE/Plugins/Fonts/Custom-test.ttf";
        else if constexpr (std::is_same_v<T, Easy2Read::FontPreset>) value = Easy2Read::FontPreset::Custom;
        else if constexpr (std::is_same_v<T, Easy2Read::LanguageSupport>) value = Easy2Read::LanguageSupport::Full;
        else if constexpr (std::is_same_v<T, float>) value = std::string_view(field.section) == "Transparency" ? 0.37f : 2.5f;
        else value = 42;
      }, field.member);
    }
    const auto untouchedTheme = Read(theme);
    Check(settings.SaveGeneral(), "General save failed");
    Check(Read(theme) == untouchedTheme, "General save changed theme INI");
    const auto savedGeneral = Read(general);
    Check(settings.SaveTheme(), "Theme save failed");
    Check(Read(general) == savedGeneral, "Theme save changed general INI");
    Check(savedGeneral.find("Keep this comment") != std::string::npos &&
          savedGeneral.find("keep-me") != std::string::npos, "General comments/unknown keys lost");
    Check(Read(theme).find("Keep theme comment") != std::string::npos &&
          Read(theme).find("keep-theme") != std::string::npos, "Theme comments/unknown keys lost");
    Check(Read(theme).find("WindowAlpha = 37") != std::string::npos, "Transparency not serialized as integer percent");
    settings.Load();
    std::size_t boolIndex = 0;
    for (const auto &field : Easy2Read::settingFields) {
      std::visit([&](auto member) {
        const auto &value = settings.*member;
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::string>) Check(value == "SKSE/Plugins/Fonts/Custom-test.ttf", "String round-trip failed");
        else if constexpr (std::is_same_v<T, Easy2Read::FontPreset>) Check(value == Easy2Read::FontPreset::Custom, "Font round-trip failed");
        else if constexpr (std::is_same_v<T, Easy2Read::LanguageSupport>) Check(value == Easy2Read::LanguageSupport::Full, "Language round-trip failed");
        else if constexpr (std::is_same_v<T, float>) Check(std::abs(value - (std::string_view(field.section) == "Transparency" ? 0.37f : 2.5f)) < 0.0001f, "Float round-trip failed");
        else if constexpr (std::is_same_v<T, bool>) Check(value == expectedBools[boolIndex++], "Boolean round-trip failed");
        else Check(value == 42, "Integer round-trip failed");
      }, field.member);
    }
    settings.fontSize = 61;
    Check(settings.LoadGeneral() && settings.fontSize == 61, "General reload changed theme settings");
    settings.toggleKey = 77;
    Check(settings.LoadTheme() && settings.toggleKey == 77, "Theme reload changed general settings");
    const auto beforeFailure = Read(general);
    Check(SetFileAttributesW(general.c_str(), FILE_ATTRIBUTE_READONLY), "Cannot make INI read-only");
    const bool savedReadOnly = settings.SaveGeneral();
    SetFileAttributesW(general.c_str(), FILE_ATTRIBUTE_NORMAL);
    Check(!savedReadOnly && Read(general) == beforeFailure, "Failed save modified existing INI");
    Check(!std::filesystem::exists(general.wstring() + L".tmp"), "Failed save left temporary file");
    std::filesystem::remove(general);
    Check(!settings.LoadGeneral() && settings.toggleKey == 77, "Missing INI changed runtime settings");
    Check(settings.SaveGeneral(), "Cannot create missing INI");
    Check(settings.LoadGeneral() && settings.toggleKey == 77, "Created INI failed round-trip");
    const auto presets = Easy2Read::Settings::ListThemePresets();
    Check(presets.size() == presetCount && presets.front() == "Default" &&
          std::ranges::find(presets, "Untarnished UI") != presets.end(),
          "Theme preset list incomplete or Default not first");
    const auto defaultPreset = Read(themes / "Default.ini");
    const auto userTheme = Read(theme);
    Check(settings.LoadThemePreset("Modern") && settings.fontPreset == Easy2Read::FontPreset::Sovngarde &&
          settings.toggleKey == 77, "Modern preset failed to load or changed general settings");
    Check(Read(theme) == userTheme, "Loading a preset changed the user theme INI");
    Check(!settings.LoadThemePreset("Missing") && settings.fontPreset == Easy2Read::FontPreset::Sovngarde,
          "Missing preset changed the current theme");
    std::filesystem::remove(theme);
    Check(settings.LoadTheme() && settings.fontPreset == Easy2Read::FontPreset::Barlow,
          "Missing user theme did not fall back to the Default preset");
    settings.fontSize = 33;
    Check(settings.SaveTheme(), "Cannot create user theme INI");
    Check(Read(themes / "Default.ini") == defaultPreset, "Saving modified the Default preset");
    Check(Read(theme).find("; Font preset:") != std::string::npos &&
          Read(theme).find("FontSize = 33") != std::string::npos,
          "New user theme lacks Default preset comments or saved values");
    { std::ofstream out(theme); out << "[Font]\nFontPreset = Sovngarde\n"; }
    Check(settings.LoadTheme() && !settings.centerTitle && !settings.showCornerOrnaments,
          "Legacy theme compatibility changed");
    using namespace Easy2Read::MenuFramework;
    Check(ToPickerKey(33, false) == ImGuiMCP::ImGuiKey_F, "F binding translation failed");
    Check(ToPickerKey(32768, true) == ImGuiMCP::ImGuiKey_GamepadFaceUp, "Y binding translation failed");
    for (bool controller : {false, true}) {
      const auto *bindings = controller ? controllerBindings : keyboardBindings;
      const auto count = controller ? std::size(controllerBindings) : std::size(keyboardBindings);
      for (std::size_t i = 0; i < count; ++i) {
        Check(FromPickerKey(ToPickerKey(bindings[i].code, controller), controller) == bindings[i].code,
              "Binding translation failed round-trip");
      }
      Check(FromPickerKey(ImGuiMCP::ImGuiKey_None, controller) == 0, "Clear binding failed");
    }
    Check(!FromPickerKey(ImGuiMCP::ImGuiKey_F, true), "Controller accepted keyboard binding");
    Check(!FromPickerKey(ImGuiMCP::ImGuiKey_GamepadFaceUp, false), "Keyboard accepted controller binding");
    Check(!FromPickerKey(ImGuiMCP::ImGuiKey_MouseLeft, false), "Keyboard accepted mouse binding");
    for (const auto &field : Easy2Read::settingFields) {
      if (const auto member = std::get_if<std::uint8_t Easy2Read::Settings::*>(&field.member)) {
        int matches = 0;
        for (const auto &color : Easy2Read::colorSettings) {
          for (auto channel : color.channels) matches += channel == *member;
        }
        Check(matches == 1, "RGB setting not covered by exactly one color picker");
      }
    }
    std::cout << "PASS: 53-setting/all-preset coverage, preset listing/loading/Default fallback, persistence, isolation, comments, failures, legacy themes, keyboard/controller binding conversions, all 8 color pickers\n";
    return 0;
  } catch (const std::exception &error) {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
