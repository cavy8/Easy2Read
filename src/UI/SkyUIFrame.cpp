#include "SkyUIFrame.h"
#include "SkyUIFrameData.h"
#include <algorithm>

namespace Easy2Read::SkyUIFrame {

int AddToAtlas(ImFontAtlas &atlas) {
  const int rectIndex = atlas.AddCustomRectRegular(Data::Width, Data::Height);
  if (!atlas.Build()) {
    return -1;
  }
  const auto rect = atlas.GetCustomRectByIndex(rectIndex);
  if (!rect->IsPacked()) {
    return -1;
  }

  unsigned char *pixels = nullptr;
  int width = 0, height = 0;
  atlas.GetTexDataAsRGBA32(&pixels, &width, &height);
  int offset = 0;
  for (const auto &run : Data::Alpha) {
    for (int i = 0; i < run.length; ++i, ++offset) {
      const int x = rect->X + offset % Data::Width;
      const int y = rect->Y + offset / Data::Width;
      const int pixel = y * width + x;
      pixels[pixel * 4] = pixels[pixel * 4 + 1] = pixels[pixel * 4 + 2] = 255;
      pixels[pixel * 4 + 3] = run.alpha;
      atlas.TexPixelsAlpha8[pixel] = run.alpha;
    }
  }
  return rectIndex;
}

void Draw(ImDrawList &drawList, ImFontAtlas &atlas, int rectIndex,
          ImVec2 position, ImVec2 size, ImU32 color, float scale) {
  if (rectIndex < 0 || size.x <= 0.0f || size.y <= 0.0f) {
    return;
  }
  const auto rect = atlas.GetCustomRectByIndex(rectIndex);
  // Keep corner proportions intact even when a custom window is very small.
  const float cornersWidth =
      (Data::Slices[0].width + Data::Slices[1].width) * 0.25f;
  const float cornersHeight =
      (Data::Slices[0].height + Data::Slices[2].height) * 0.25f;
  scale = (std::max)(0.0f, (std::min)(scale,
      (std::min)(size.x / cornersWidth, size.y / cornersHeight)));
  const float left = Data::Slices[0].width * 0.25f * scale;
  const float right = Data::Slices[1].width * 0.25f * scale;
  const float top = Data::Slices[0].height * 0.25f * scale;
  const float bottom = Data::Slices[2].height * 0.25f * scale;
  const float x[] = {position.x, position.x + left,
                     position.x + size.x - right, position.x + size.x};
  const float y[] = {position.y, position.y + top,
                     position.y + size.y - bottom, position.y + size.y};
  // TL, TR, BL, BR, top, bottom, left, right, in generated atlas order.
  constexpr int cells[][2] = {{0, 0}, {2, 0}, {0, 2}, {2, 2},
                              {1, 0}, {1, 2}, {0, 1}, {2, 1}};
  drawList.PushClipRect(position, ImVec2(x[3], y[3]));
  for (int i = 0; i < 8; ++i) {
    const auto &slice = Data::Slices[i];
    const int column = cells[i][0], row = cells[i][1];
    // Stretch only between texel centers: sampling the transparent gutters
    // would turn a half-texel transition into a long fade near each corner.
    const float insetX = (i == 4 || i == 5) ? 0.5f : 0.0f;
    const float insetY = (i == 6 || i == 7) ? 0.5f : 0.0f;
    const ImVec2 uvMin(
        (rect->X + slice.x + insetX) / static_cast<float>(atlas.TexWidth),
        (rect->Y + slice.y + insetY) / static_cast<float>(atlas.TexHeight));
    const ImVec2 uvMax(
        (rect->X + slice.x + slice.width - insetX) / static_cast<float>(atlas.TexWidth),
        (rect->Y + slice.y + slice.height - insetY) / static_cast<float>(atlas.TexHeight));
    drawList.AddImage(atlas.TexID, ImVec2(x[column], y[row]),
                      ImVec2(x[column + 1], y[row + 1]), uvMin, uvMax, color);
  }
  drawList.PopClipRect();
}

} // namespace Easy2Read::SkyUIFrame
