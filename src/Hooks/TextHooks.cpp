#include "TextHooks.h"
#include "Config/Settings.h"
#include "PCH.h"
#include "TextSanitization/TextSanitizer.h"
#include <xbyak/xbyak.h>

// Hook helper functions (pattern from Dynamic String Distributor)
namespace stl {
using namespace SKSE::stl;

template <class T, size_t size = 5>
void write_thunk_call(std::uintptr_t a_src) {
  auto &trampoline = SKSE::GetTrampoline();
  T::func = trampoline.write_call<size>(a_src, T::thunk);
}

template <class T, std::size_t BYTES>
void hook_function_prologue(std::uintptr_t a_src) {
  struct Patch : Xbyak::CodeGenerator {
    Patch(std::uintptr_t a_originalFuncAddr, std::size_t a_originalByteLength) {
      // Execute restored bytes and jump back to original function
      for (size_t i = 0; i < a_originalByteLength; ++i) {
        db(*reinterpret_cast<std::uint8_t *>(a_originalFuncAddr + i));
      }
      jmp(ptr[rip]);
      dq(a_originalFuncAddr + a_originalByteLength);
    }
  };

  Patch p(a_src, BYTES);
  p.ready();

  auto &trampoline = SKSE::GetTrampoline();
  trampoline.write_branch<5>(a_src, T::thunk);

  auto alloc = trampoline.allocate(p.getSize());
  std::memcpy(alloc, p.getCode(), p.getSize());

  T::func = reinterpret_cast<std::uintptr_t>(alloc);
}
} // namespace stl

namespace Easy2Read {

// ============================================================================
// GetDescriptionHook - DESC/CNAM records (books, items, spells, etc.)
// ============================================================================
void TextHooks::GetDescriptionHook::thunk(RE::TESDescription *a_desc,
                                          RE::BSString &a_out,
                                          const RE::TESForm *a_parent,
                                          std::uint32_t a_chunkID) {
  // Call original function first
  func(a_desc, a_out, a_parent, a_chunkID);

  // In safe mode, only process Book forms to avoid crashes with other types
  // But if parent is null, we can't check form type - still process since book
  // text often has null parent
  auto *settings = Settings::GetSingleton();
  if (settings->sanitizationSafeMode && a_parent) {
    if (a_parent->GetFormType() != RE::FormType::Book) {
      return;
    }
  }

  // Sanitize the output
  auto *sanitizer = TextSanitizer::GetSingleton();
  if (sanitizer->IsEnabled() && a_out.length() > 0) {
    std::string sanitized = sanitizer->Sanitize(a_out.c_str());
    if (sanitized != a_out.c_str()) {
      a_out = sanitized;
      SKSE::log::debug("TextHooks: Sanitized DESC for form {:08X}",
                       a_parent ? a_parent->GetFormID() : 0);
    }
  }
}

void TextHooks::GetDescriptionHook::Install() {
  // Hook TESDescription::GetDescription
  // SE: 14401, AE: 14552, VR: 0x1A0300
  REL::Relocation<std::uintptr_t> target{
      REL::VariantID(14401, 14552, 0x1A0300)};
  stl::hook_function_prologue<GetDescriptionHook, 6>(target.address());
  SKSE::log::info("TextHooks: GetDescriptionHook installed");
}

// ============================================================================
// DialogueResponseHook - INFO NAM1 (dialogue subtitles)
// ============================================================================
RE::DialogueResponse *TextHooks::DialogueResponseHook::thunk(
    RE::DialogueResponse *a_item, RE::TESTopic *a_topic,
    RE::TESTopicInfo *a_topicInfo, RE::TESObjectREFR *a_speaker,
    RE::TESTopicInfo::ResponseData *a_response) {
  // Call original function first
  auto *result = func(a_item, a_topic, a_topicInfo, a_speaker, a_response);

  // Sanitize the dialogue text
  auto *sanitizer = TextSanitizer::GetSingleton();
  if (sanitizer->IsEnabled() && result && result->text.length() > 0) {
    std::string sanitized = sanitizer->Sanitize(result->text.c_str());
    if (sanitized != result->text.c_str()) {
      result->text = sanitized;
      SKSE::log::debug("TextHooks: Sanitized dialogue for INFO {:08X}",
                       a_topicInfo ? a_topicInfo->GetFormID() : 0);
    }
  }

  return result;
}

void TextHooks::DialogueResponseHook::Install() {
  // Hook dialogue response creation
  // SE: 34413, AE: 35220
  REL::Relocation<std::uintptr_t> target1{RELOCATION_ID(34413, 35220), 0xE4};
  stl::write_thunk_call<DialogueResponseHook>(target1.address());

  REL::Relocation<std::uintptr_t> target2{
      REL::VariantID(34436, 35256, 0x574360), 0xCD};
  stl::write_thunk_call<DialogueResponseHook>(target2.address());

  // AE has additional call site
  if (REL::Module::IsAE()) {
    REL::Relocation<std::uintptr_t> target3{REL::ID(35254), 0x2ED};
    stl::write_thunk_call<DialogueResponseHook>(target3.address());
  }

  SKSE::log::info("TextHooks: DialogueResponseHook installed");
}

// ============================================================================
// DialogueMenuTextHook - DIAL FULL, INFO RNAM (dialogue menu options)
// ============================================================================
void TextHooks::DialogueMenuTextHook::thunk(
    RE::MenuTopicManager::Dialogue &a_out, char *a_buffer,
    std::uint64_t a_unk) {
  // Call original function first
  func(a_out, a_buffer, a_unk);

  // Sanitize the topic text
  auto *sanitizer = TextSanitizer::GetSingleton();
  if (sanitizer->IsEnabled() && a_out.topicText.length() > 0) {
    std::string sanitized = sanitizer->Sanitize(a_out.topicText.c_str());
    if (sanitized != a_out.topicText.c_str()) {
      a_out.topicText = sanitized;
      SKSE::log::debug("TextHooks: Sanitized dialogue menu text");
    }
  }
}

void TextHooks::DialogueMenuTextHook::Install() {
  // Hook dialogue menu text setting
  // SE: 34434, AE: 35254
  REL::Relocation<std::uintptr_t> target1{RELOCATION_ID(34434, 35254),
                                          REL::Relocate(0xCC, 0x226)};
  stl::write_thunk_call<DialogueMenuTextHook>(target1.address());

  // AE has additional call site
  if (REL::Module::IsAE()) {
    REL::Relocation<std::uintptr_t> target2{REL::ID(35254), 0x115};
    stl::write_thunk_call<DialogueMenuTextHook>(target2.address());
  }

  SKSE::log::info("TextHooks: DialogueMenuTextHook installed");
}

// ============================================================================
// MapMarkerDataHook - REFR FULL (map marker names)
// ============================================================================
RE::TESFullName *
TextHooks::MapMarkerDataHook::thunk(RE::TESObjectREFR *a_marker) {
  if (!a_marker || a_marker->IsDisabled()) {
    return func(a_marker);
  }

  auto *result = func(a_marker);

  // Detect unsupported characters (cannot modify TESFullName in-place)
  auto *sanitizer = TextSanitizer::GetSingleton();
  if (sanitizer->IsEnabled() && result) {
    const char *name = result->GetFullName();
    if (name && name[0] != '\0' && sanitizer->NeedsSanitization(name)) {
      SKSE::log::debug("TextHooks: Map marker {:08X} has unsupported chars",
                       a_marker->GetFormID());
    }
  }

  return result;
}

void TextHooks::MapMarkerDataHook::Install() {
  // Hook map marker data retrieval
  // SE: 18755, AE: 19216
  REL::Relocation<std::uintptr_t> target1{RELOCATION_ID(18755, 19216),
                                          REL::Relocate(0xA6, 0xE4)};
  stl::write_thunk_call<MapMarkerDataHook>(target1.address());

  SKSE::log::info("TextHooks: MapMarkerDataHook installed");
}

// ============================================================================
// NpcNameHook - NPC FULL (NPC names during file load)
// ============================================================================
void TextHooks::NpcNameHook::thunk(RE::TESFullName *a_fullname,
                                   RE::TESFile *a_file) {
  func(a_fullname, a_file);

  // Detect unsupported characters (cannot modify TESFullName in-place)
  auto *sanitizer = TextSanitizer::GetSingleton();
  if (sanitizer->IsEnabled()) {
    const char *name = a_fullname->GetFullName();
    if (name && name[0] != '\0' && sanitizer->NeedsSanitization(name)) {
      SKSE::log::debug("TextHooks: NPC name has unsupported chars");
    }
  }
}

void TextHooks::NpcNameHook::Install() {
  // Hook NPC name file stream
  // SE: 24159, AE: 24663
  REL::Relocation<std::uintptr_t> target1{RELOCATION_ID(24159, 24663),
                                          REL::Relocate(0x7CE, 0x924)};
  stl::write_thunk_call<NpcNameHook>(target1.address());

  SKSE::log::info("TextHooks: NpcNameHook installed");
}

// ============================================================================
// QuestTextHook - QUST CNAM (quest journal descriptions)
// ============================================================================
void TextHooks::QuestTextHook::thunk(RE::BSString &a_out, char *a_buffer,
                                     std::uint64_t a_unk) {
  // Call original function first
  func(a_out, a_buffer, a_unk);

  // Sanitize the quest description text
  auto *sanitizer = TextSanitizer::GetSingleton();
  if (sanitizer->IsEnabled() && a_out.length() > 0) {
    std::string original = a_out.c_str();
    std::string sanitized = sanitizer->Sanitize(original);
    if (sanitized != original) {
      a_out = sanitized;
      SKSE::log::debug("TextHooks: Sanitized quest description");
    }
  }
}

void TextHooks::QuestTextHook::Install() {
  // Hook quest description text (QUST CNAM)
  // SE: 24778, AE: 25259
  REL::Relocation<std::uintptr_t> target1{RELOCATION_ID(24778, 25259),
                                          REL::Relocate(0x21C, 0x221)};
  stl::write_thunk_call<QuestTextHook>(target1.address());

  SKSE::log::info("TextHooks: QuestTextHook installed");
}

// ============================================================================
// Main Install function
// ============================================================================
void TextHooks::Install() {
  SKSE::log::info("TextHooks: Installing text sanitization hooks...");

  // Get hook settings from sanitizer
  auto *sanitizer = TextSanitizer::GetSingleton();
  const auto &hooks = sanitizer->GetHookSettings();

  if (hooks.enableDescription) {
    GetDescriptionHook::Install();
  }
  if (hooks.enableDialogue) {
    DialogueResponseHook::Install();
  }
  if (hooks.enableDialogueMenu) {
    DialogueMenuTextHook::Install();
  }
  if (hooks.enableMapMarker) {
    MapMarkerDataHook::Install();
  }
  if (hooks.enableNpcName) {
    NpcNameHook::Install();
  }
  if (hooks.enableQuest) {
    QuestTextHook::Install();
  }

  SKSE::log::info("TextHooks: All hooks installed successfully");
}

} // namespace Easy2Read
