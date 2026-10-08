#pragma once

// Keep the framework API translation unit independent of local ImGui types.
namespace Easy2Read {
void RequestOverlayResourceRefresh(bool fonts = true);
void HideReadingOverlay();
} // namespace Easy2Read
