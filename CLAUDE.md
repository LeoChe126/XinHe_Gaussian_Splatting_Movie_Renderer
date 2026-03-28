# GSMovie Plugin Development

## Project Structure
- **Local path:** `D:\Github\splatrenderer\` (renamed from csgs-release)
- **Source code:** `GSMovie/Source/CSGaussianRenderer/` — active development here
- **Shaders:** `GSMovie/Shaders/Private/`
- **Binaries:** `GSMovie/Binaries/Win64/` (precompiled DLL for release)
- **Blueprints:** `GSMovie/Content/Blueprints/`
- **Notes/plans:** `notes/`
- **Reference plugins:** `ref/` (PICOSplat, Postshot, Volinga)

## UE Plugin
- UE loads the plugin via symlink from `D:\Unreal Projects\FourDGS - V2\Plugins\GSMovie\` → this folder's `GSMovie/`
- Edit source here, UE compiles from here via the symlink

## Git History
- **This repo** (`GSMovie-UEPlugin`, public): Release only — DLL, shaders, content, docs. Source/ is gitignored.
- **Old dev history** (`csgs.git`): at `D:\Unreal Projects\FourDGS - V2\Plugins\CSGaussianRenderer\.git` — check there for commit history before this consolidation

## Build
- Build script: `cmd.exe //C "C:\Users\tommy\AppData\Local\Temp\build_csg3.bat"`
- UE Editor must be closed for linking; shader-only changes just need editor restart
- NEVER run two builds simultaneously

## Key Rules
- Do NOT push Source/ to this public repo
- Module name is still `CSGaussianRenderer` (rename to GSMovie planned)
- User communicates in Traditional Chinese
- Analyze before building — wait for permission before running builds
