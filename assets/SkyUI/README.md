# SkyUI message-box frame

The frame artwork is extracted from SkyUI / SkyUI Community, by the SkyUI team,
doodlum, and contributors. Used under SkyUI's open asset permissions; SkyUI icons
are excluded from this extraction.

- Repository: https://github.com/doodlum/SkyUI-Community
- Revision: `b7a24a936647e815269e34f4899e4b09768eff29`
- Source: `data/interface/messagebox.swf`, shape 10, used by the `Background_mc`
  instance of `MessageBox` (sprite 15 via sprite 11).
- Inspectable source: `source/swf/messagebox.xml` at the same revision.

`messagebox-frame.svg` was exported with JPEXS Free Flash Decompiler 26.0.0:

```powershell
java -jar tools/FFDec/ffdec-cli.jar -selectid 10 -format shape:svg -export shape <output> data/interface/messagebox.swf
```

Only the gray decorative fill is retained; the solid black background path is
removed because Easy2Read draws its configurable translucent window background.
All corner geometry and curves come from the source artwork.

The original frame bounds are `(-4324,-1550)..(4322,1546)` in Flash twips.
Its scaling grid is `(-3751,-924)..(3724,960)`. The generator renders at 384 DPI
(4x) and packs four corners and four narrow border strips into a 256x512 alpha
atlas. ImGui draws these slices with fixed corner proportions and stretches
only the middle strips. Color and opacity remain configurable.
Stretched strips sample between texel centers along their stretched axis so
transparent atlas gutters cannot fade the border near its joins with the corners.

To regenerate `src/UI/SkyUIFrameData.h`, install Pillow and Inkscape and run:

```powershell
python tools/generate-skyui-frame.py --inkscape "C:/Program Files/Inkscape/bin/inkscape.com"
```

The alpha atlas is embedded in the DLL and uploaded with the font atlas; players
do not need the SVG, Inkscape, JPEXS, or a separate texture file. Attribution is
also bundled in `Data/SKSE/Plugins/Easy2Read/Licenses/SkyUI Attribution.txt`.
