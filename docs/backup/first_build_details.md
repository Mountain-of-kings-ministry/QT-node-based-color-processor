Everything builds and links successfully. Here's a summary of what was implemented:

---

## Build Output

| Target | Type | Size |
|---|---|---|
| `MOKMColorProcessor` | GUI application | 2.2MB |
| `mokm-cli` | Headless CLI tool | 194KB |
| `libMOKMColorLib.a` | Core library (QtCore + OCIO) | 266KB |
| `libMOKMColorNodes.a` | Node implementations | 356KB |
| `libMOKMColorGUI.a` | GUI components | 44KB |

## Files Created/Modified (32 files)

### Core Library (`src/core/`, `include/mokm/`)
- **ImageBuffer** (`include/mokm/ImageBuffer.h`, `src/core/ImageBuffer.cpp`) — Float32 pixel buffer with load/save/ROI/clone
- **OCIOWrapper** (`include/mokm/OCIOWrapper.h`, `src/core/OCIOWrapper.cpp`) — Singleton wrapping OCIO config loading, processor caching, display/view transforms
- **PipelineExecutor** (`include/mokm/PipelineExecutor.h`, `src/core/PipelineExecutor.cpp`) — Graph-based image/sequence processing engine with topological sort
- **NodeGraph** (`include/mokm/NodeGraph.h`, `src/core/NodeGraph.cpp`) — Serializable graph description (JSON) for headless use
- **MOKMVersion** (`include/mokm/MOKMVersion.h`) — Version constants

### OCIO Nodes (`src/nodes/ocio/`)
- **ConfigLoaderNode** — Load .ocio config file
- **ColorSpaceConvertNode** — Convert between color spaces
- **DisplayViewNode** — Apply display + view transform
- **FileTransformNode** — Load and apply LUT files
- **LookTransformNode** — Apply creative looks
- **RangeTransformNode** — Clamp/normalize pixel values

### IO & Output Nodes (`src/nodes/io/`, `src/nodes/output/`)
- **ImageLoaderNode** / **ImageSaverNode** — Image file I/O
- **PreviewNode** — Output to preview window with callback

### Registration (`src/nodes/registerMOKMNodes.h/.cpp`)
- Registers all 9 nodes under "MOKM" category with gold accent color

### GUI (`src/ui/`)
- **PreviewWindow** — Frameless floating preview with zoom/pan, display/view selectors, exposure slider, themed dark UI

### QML Pages (`src/pages/`)
- **Main.qml** — Full node editor workspace with sidebar, properties panel, OCIO config dialog
- **SplashScreen.qml** — Updated for MOKM branding, app name, theme colors

### CLI (`tools/mokm-cli/`)
- **`mokm-cli`** — Headless mode with `--graph`, `--input`/`--output`, `--sequence`/`--range`, `--config`, `--list-cs`

### CMake Restructure
- Root `CMakeLists.txt` — OCIO + NodeEditor + lib/app targets with `BUILD_GUI` / `BUILD_HEADLESS` options
- `src/CMakeLists.txt` — Subdirectory dispatch
- `src/core/CMakeLists.txt` — `MOKMColorLib` (QtCore + OCIO)
- `src/nodes/CMakeLists.txt` — `MOKMColorNodes` (NodeEditorCore + MOKMColorLib)
- `src/ui/CMakeLists.txt` — `MOKMColorGUI` + `MOKMColorProcessor` executable
- Updated `main.cpp` — Node registration + preview callback + OCIO init

### Theme Integration
- All GUI components use `Theme.qml` colors (gold accents, dark backgrounds)
- PreviewWindow styled with matching dark theme