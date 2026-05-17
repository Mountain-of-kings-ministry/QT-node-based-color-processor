Everything builds and links cleanly against Qt 6.11. Here's what was fixed:

### Changes Made

| File | Fix |
|---|---|
| `CMakeLists.txt` | Added `QT_RESOURCE_ALIAS` for all 3 QML files; added `target_include_directories` + `target_link_libraries` for `appMOKM_Color_Processor`; cleaned up target ordering (executable before QML module) |
| `src/ui/CMakeLists.txt` | Removed `qt_add_executable(MOKMColorProcessor...)` block that conflicted with root target; kept only `MOKMColorGUI` static library |
| `main.cpp` | Fixed `qmlEngine.loadFromModule` → `engine.loadFromModule` |

### Build Output (Qt 6.11 preset)

| Artifact | Path | Size |
|---|---|---|
| `appMOKM_Color_Processor` | `build/default/bin/` | 6.1MB |
| `mokm-cli` | `build/default/bin/` | 186KB |
| `libMOKMColorLib.a` | `build/default/lib/` | 255KB |
| `libMOKMColorNodes.a` | `build/default/lib/` | 351KB |
| `libMOKMColorGUI.a` | `build/default/lib/` | 45KB |

To build: `cmake --preset default && cmake --build --preset default`
