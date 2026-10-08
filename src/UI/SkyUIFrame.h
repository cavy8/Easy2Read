#pragma once

#include <imgui.h>

namespace Easy2Read::SkyUIFrame {
// Builds the font atlas and adds SkyUI's frame artwork before the GPU upload.
int AddToAtlas(ImFontAtlas &atlas);
void Draw(ImDrawList &drawList, ImFontAtlas &atlas, int rectIndex,
          ImVec2 position, ImVec2 size, ImU32 color, float scale);
} // namespace Easy2Read::SkyUIFrame
