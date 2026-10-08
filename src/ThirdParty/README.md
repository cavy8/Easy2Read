# SKSE Menu Framework API

`SKSEMenuFramework.h` is vendored unmodified from the current framework's
`resources/SKSEMenuFramework.h`, revision `dc9182454556910b2fd873d770c345cfe6e53777`:
https://github.com/QTR-Modding/SKSE-Menu-Framework-3

The separate API repository identifies the API's license as LGPL-2.1:
https://github.com/QTR-Modding/SKSE-Menu-Framework-3-API
Its license is included as `SKSEMenuFramework.LICENSE` and shipped in
`Data/SKSE/Plugins/Easy2Read/Licenses/SKSEMenuFramework-LGPL-2.1.txt`.

The current framework resource header contains `ImGuiMCPComponents::KeyBindingPicker`;
the older header in the separate API repository does not. The binding translation
table in `src/UI/MenuBindings.h` follows the framework's `src/Input.cpp` at the
same revision. The framework project is copyright its contributors (QTR Modding /
Thiago099).

Only the framework editor includes this header. Its `ImGuiMCP` wrappers call the
installed framework's exports; Easy2Read's reading overlay keeps its own ImGui
context and linked implementation.
