#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>

namespace Easy2Read {

class BookPromptIcon {
public:
  void Load(std::uint32_t button, bool gamepad);
  [[nodiscard]] ID3D11ShaderResourceView *GetTexture() const { return texture.Get(); }
  [[nodiscard]] float GetAspectRatio() const { return aspectRatio; }

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  float aspectRatio = 1.0f;
};

} // namespace Easy2Read
