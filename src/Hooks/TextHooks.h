#pragma once

namespace Easy2Read {

/// Text hooks for sanitizing game text at the record level.
/// Based on Dynamic String Distributor's approach for comprehensive coverage.
class TextHooks {
public:
  /// Install all text sanitization hooks
  static void Install();

private:
  /// Hook for TESDescription::GetDescription - catches DESC/CNAM records
  /// Covers: books, items, spells, perks, etc.
  struct GetDescriptionHook {
    static void thunk(RE::TESDescription *a_desc, RE::BSString &a_out,
                      const RE::TESForm *a_parent, std::uint32_t a_chunkID);
    static inline REL::Relocation<decltype(thunk)> func;
    static void Install();
  };

  /// Hook for dialogue response text - INFO NAM1 records
  /// Covers: dialogue subtitles displayed during conversations
  struct DialogueResponseHook {
    static RE::DialogueResponse *
    thunk(RE::DialogueResponse *a_item, RE::TESTopic *a_topic,
          RE::TESTopicInfo *a_topicInfo, RE::TESObjectREFR *a_speaker,
          RE::TESTopicInfo::ResponseData *a_response);
    static inline REL::Relocation<decltype(thunk)> func;
    static void Install();
  };

  /// Hook for dialogue menu text - DIAL FULL, INFO RNAM
  /// Covers: dialogue menu options shown to player
  struct DialogueMenuTextHook {
    static void thunk(RE::MenuTopicManager::Dialogue &a_out, char *a_buffer,
                      std::uint64_t a_unk);
    static inline REL::Relocation<decltype(thunk)> func;
    static void Install();
  };

  /// Hook for map marker names - REFR FULL
  /// Covers: map marker location names
  struct MapMarkerDataHook {
    static RE::TESFullName *thunk(RE::TESObjectREFR *a_marker);
    static inline REL::Relocation<decltype(thunk)> func;
    static void Install();
  };

  /// Hook for NPC names during file load - NPC FULL
  /// Covers: NPC display names
  struct NpcNameHook {
    static void thunk(RE::TESFullName *a_fullname, RE::TESFile *a_file);
    static inline REL::Relocation<decltype(thunk)> func;
    static void Install();
  };

  /// Hook for quest description text - QUST CNAM
  /// Covers: quest journal descriptions
  struct QuestTextHook {
    static void thunk(RE::BSString &a_out, char *a_buffer, std::uint64_t a_unk);
    static inline REL::Relocation<decltype(thunk)> func;
    static void Install();
  };
};

} // namespace Easy2Read
