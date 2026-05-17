I've now read all the docs, the existing project structure, third-party libraries, and source code. Here's a comprehensive plan.

---

## Implementation Plan: MOKM Color Processor

### Current State Summary
- **CMake**: QML-only app (`applearConnection_2`), no OCIO or NodeEditor linked yet
- **Node Editor**: Submodule at `third_parties/node/` — has `NodeEditorCore` (C++ static lib) + `NodeEditor` (QML module). Uses `BaseNode::compute()`, `GraphModel`, `DataFlowEngine`
- **OpenColorIO**: v2.5.2 fully built + installed to `third_parties/install/` (shared libs ready)
- **Theme**: Full gold-accented dark theme in `src/pages/Theme.qml` (singleton)
- **Directories**: `include/nodes/`, `include/connectors/` exist but empty; `src/core/`, `src/nodes/`, `src/ui/` don't exist yet
- **Pages**: SplashScreen → Main.qml (simple button demo)

---

### Phase 0 — Restructure CMake & Dependencies (must fix first)

**Goal**: Get OCIO + NodeEditor properly linked, rename targets, set up library/GUI separation.

1. **Rewrite root `CMakeLists.txt`**:
   - Rename project to `MOKM_Color_Processor`
   - Add `find_package(OpenColorIO REQUIRED CONFIG)` pointing to `third_parties/install`
   - Add `add_subdirectory(third_parties/node/NodeEditor)` for the node editor
   - Add options: `BUILD_GUI` (ON), `BUILD_HEADLESS` (ON)

2. **Create `src/CMakeLists.txt` with sub-libraries**:
   - `MOKMColorLib` — static/shared library, **no Qt dep** (core + nodes + pipeline)
     - Sources in `src/core/` and `src/nodes/`
   - `MOKMColorGUI` — static/shared library, Qt + QML dep
     - Sources in `src/ui/` and existing `src/pages/`
   - Link: `MOKMColorGUI` → `MOKMColorLib` → `OpenColorIO::OpenColorIO`
   - Link the main executable to `MOKMColorGUI` and `NodeEditor::NodeEditor`

3. **Fix existing CMake inconsistencies**:
   - Remove old `applearConnection_2` / `learConnection_2` names
   - Update QML module URI to `MOKM_Color_Processor`
   - Register Theme.qml as singleton properly

---

### Phase 1 — Core Library (`MOKMColorLib`, no Qt)

**Goal**: Headless core usable as a C++ library for external integration.

#### 1.1 ImageBuffer (`src/core/ImageBuffer.h/.cpp`)
```cpp
class ImageBuffer {
    int width, height, channels;
    std::vector<float> data;  // float32 pixels
    QString colorSpace;       // source colorspace name
    // Constructors: from file, from raw data, allocate
    // Methods: ROI(), convertToQImage(), thumbnail()
};
```

#### 1.2 OCIOWrapper (`src/core/OCIOWrapper.h/.cpp`)
- Singleton managing OCIO config lifecycle
- Methods: `loadConfig(path)`, `getProcessor(src, dst)`, `apply(processor, buffer)`
- Processor cache (`QHash` of config + src + dst → `ConstProcessorRcPtr`)
- Context variable support

#### 1.3 Pipeline Executor (`src/core/PipelineExecutor.h/.cpp`)
- Takes a `NodeGraph` description (serializable)
- Topological sort + lazy evaluation
- Dirty-flag propagation
- Headless image and sequence processing
- `ProcessImage(graph, buffer)` and `ProcessSequence(graph, sequence)`

#### 1.4 Public API Headers (`include/`)
- `include/mokm/ColorPipeline.h` — main entry point
- `include/mokm/ImageBuffer.h` — image data
- `include/mokm/NodeGraph.h` — graph definition
- `include/mokm/NodeBase.h` — node interface (can be separate from NodeEditor's BaseNode for headless)

---

### Phase 2 — OCIO Nodes

**Goal**: Implement color processing nodes that wrap OCIO.

Follow the NodeEditor's `BaseNode` pattern. Each node:
- Inherits `NodeEditor::BaseNode`
- Overrides `compute()`, `inputSpec()`, `outputSpec()`, `nodeType()`
- Uses OCIO internally

#### MVP Nodes (Phase 2a):
| Node | File | Description |
|---|---|---|
| **OCIOConfigLoader** | `src/nodes/ocio/ConfigLoaderNode.h/.cpp` | Load .ocio file, output config ref |
| **ColorSpaceConvert** | `src/nodes/ocio/ColorSpaceConvertNode.h/.cpp` | Convert between two color spaces |
| **DisplayView** | `src/nodes/ocio/DisplayViewNode.h/.cpp` | Apply display + view transform |
| **LookTransform** | `src/nodes/ocio/LookTransformNode.h/.cpp` | Apply a named look |
| **FileTransform** | `src/nodes/ocio/FileTransformNode.h/.cpp` | Load & apply LUT (.cube, .3dl, etc.) |
| **ImageLoader** | `src/nodes/io/ImageLoaderNode.h/.cpp` | Load image from disk → ImageBuffer |
| **ImageSaver** | `src/nodes/io/ImageSaverNode.h/.cpp` | Save ImageBuffer to disk |
| **Preview** | `src/nodes/output/PreviewNode.h/.cpp` | Emit signal for preview window |
| **RangeTransform** | `src/nodes/ocio/RangeTransformNode.h/.cpp` | Clamp/normalize |

#### Grading Nodes (Phase 2b):
| Node | Description |
|---|---|
| **CDL** | ASC CDL (Slope, Offset, Power, Saturation) |
| **Grade** | Lift/Gamma/Gain + Offset + Saturation |
| **Exposure** | Exposure compensation |
| **Contrast** | Contrast adjustment |
| **Saturation** | Saturation adjustment |
| **Curve** | RGB curve editor |
| **HueCorrect** | Hue-specific corrections |

#### Utility Nodes (Phase 2c):
| Node | Description |
|---|---|
| **Switch** | Multi-input selector |
| **Merge/Blend** | Over, add, multiply, screen, etc. |
| **Crop/ROI** | Region of interest |
| **Resize** | Image scaling |
| **Premult/Unpremult** | Alpha handling |
| **ChannelSplit** | Split RGBA |
| **ChannelJoin** | Combine channels |

#### Data Types to Register with NodeEditor:
- `ImageData` (custom `PortType::Image` → maps to `std::shared_ptr<ImageBuffer>`)
- `ColorSpaceData` (string naming a colorspace from current config)
- `OCIOConfigData` (config reference)

---

### Phase 3 — GUI Application

**Goal**: Full Qt application with node editor workspace, preview, and inspectors.

#### 3.1 Main Window (`src/ui/MainWindow.qml` or `MainWindow.cpp`)
- Replace current `Main.qml` with a proper app layout
- Uses `NodeEditorWorkspace` from the node editor
- Applies `Theme` singleton for all colors
- Sidebar: node palette, properties inspector
- Menu bar: File (load/save graph), Config (load .ocio), Render (export)

#### 3.2 Theme Integration
- Map `Theme.qml` colors into the NodeEditor QML components
- In `NodeCanvas.qml`, `Node.qml`, `Edge.qml`, etc., reference `Theme.background`, `Theme.primary`, etc.
- Register Theme as a QML singleton (already done in CMake)

#### 3.3 Floating Preview Window (`src/ui/PreviewWindow.h/.cpp` + `src/ui/PreviewWindow.qml`)
- Frameless window (`Qt::FramelessWindowHint | Qt::Tool`)
- Use the existing design from docs/plan_first.md
- Connected to `PreviewNode` via signal/slot (ImageBuffer ready)
- Features: OCIO display/view selector, zoom/pan, pixel probe, exposure slider
- Stays within main window bounds

#### 3.4 Node Inspectors
- For each OCIO node, a QML inspector showing relevant controls:
  - `ColorSpaceConvert`: dropdowns of available color spaces (from config)
  - `DisplayView`: dropdowns for display + view
  - `CDL`: sliders for slope/offset/power/saturation
  - `ImageLoader`: file browser
- Use `PropertiesPanel` mechanism already in NodeEditor

#### 3.5 Register All Nodes with GraphModel
```cpp
// in main.cpp or app init
GraphModel *model = new GraphModel();
registerDefaultNodeTypes(model);  // built-in node editor nodes
registerMOKMNodeTypes(model);    // our OCIO nodes
NodeEditor::registerMOKMNodeTypes(model);
```

---

### Phase 4 — Headless Mode & Library API

**Goal**: Process images from command line or embed in other apps.

#### 4.1 Command-Line Interface
```bash
mokm-process --graph pipeline.json --input image.exr --output result.exr
mokm-process --graph pipeline.json --sequence frame.%04d.exr --range 1-100
```

#### 4.2 C++ Library API (`include/mokm/`)
```cpp
namespace mokm {
    struct PipelineDesc {
        std::string graphJson;  // serialized node graph
        std::string ocioConfigPath;
    };
    
    class ColorPipeline {
    public:
        static std::unique_ptr<ColorPipeline> Create(const PipelineDesc& desc);
        bool ProcessImage(ImageBuffer& buffer);
        bool ProcessSequence(const std::string& pattern, int start, int end);
    };
}
```

#### 4.3 CMake Export
- Install `MOKMColorLib` with public headers
- Generate `MOKMColorLibConfig.cmake` for `find_package(MOKMColorLib)`
- Zero Qt dependency for library consumers

---

### Phase 5 — Video Pipeline

**Goal**: Image sequence / video clip processing.

- `SequenceLoaderNode` — loads frames with frame range, padding, FPS
- Frame cache (LRU cache of `ImageBuffer` for recent frames)
- Per-frame graph evaluation (reuse OCIO processors across frames)
- `VideoWriterNode` — write to EXR sequence or video file (future)
- `PreviewNode` real-time playback at target FPS

---

### Phase 6 — Advanced Features

- **GPU Processing**: Use OCIO's `GPUProcessor` with OpenGL/Vulkan compute
- **LUT Baker**: `LUTBakerNode` — bake sub-graph into 3D LUT (.cube, .spi3d)
- **Group/Compound Nodes**: Reusable sub-graphs with custom ports
- **Analysis Nodes**: Histogram, waveform, vectorscope, pixel probe
- **Plugin System**: Dynamic loading of custom node `.so` files

---

### Priority & Ordering

```
Week 1-2:  Phase 0 (CMake restructure) + Phase 1 (Core Library)
Week 2-3:  Phase 2a (MVP OCIO nodes: Config, ColorSpaceConvert, DisplayView, ImageLoader, Preview)
Week 3-4:  Phase 3 (GUI: MainWindow, PreviewWindow, inspectors, Theme integration)
Week 4-5:  Phase 2b-2c (Grading + Utility nodes)
Week 5-6:  Phase 4 (Headless mode, CLI, library API)
Week 7+:   Phase 5 (Video), Phase 6 (GPU, LUT bake, plugins, analysis)
```

---

### Files to Create (initial pass)

| File | Purpose |
|---|---|
| `src/core/ImageBuffer.h/.cpp` | Pixel buffer with float32 support |
| `src/core/OCIOWrapper.h/.cpp` | OCIO config/processor management |
| `src/core/PipelineExecutor.h/.cpp` | Graph eval engine |
| `src/core/NodeGraph.h/.cpp` | Serializable graph model for headless |
| `include/mokm/ColorPipeline.h` | Public API for library consumers |
| `include/mokm/ImageBuffer.h` | Public ImageBuffer header |
| `include/mokm/NodeGraph.h` | Public graph descriptor |
| `include/mokm/MOKMVersion.h` | Version constants |
| `src/nodes/ocio/ConfigLoaderNode.h/.cpp` | OCIO config node |
| `src/nodes/ocio/ColorSpaceConvertNode.h/.cpp` | Colorspace conversion node |
| `src/nodes/ocio/DisplayViewNode.h/.cpp` | Display/view transform node |
| `src/nodes/ocio/FileTransformNode.h/.cpp` | LUT file load node |
| `src/nodes/ocio/LookTransformNode.h/.cpp` | Look apply node |
| `src/nodes/ocio/RangeTransformNode.h/.cpp` | Range clamp node |
| `src/nodes/io/ImageLoaderNode.h/.cpp` | Image file loader |
| `src/nodes/io/ImageSaverNode.h/.cpp` | Image file saver |
| `src/nodes/output/PreviewNode.h/.cpp` | Preview output |
| `src/nodes/registerMOKMNodes.h/.cpp` | Registration function |
| `src/ui/PreviewWindow.h/.cpp` | Frameless preview widget |
| `src/ui/PreviewWindow.qml` | Preview QML overlay |
| `src/ui/MainWindow.qml` | Replace current Main.qml |
| `main.cpp` | Update with OCIO + NodeEditor init |
| `tools/mokm-cli/main.cpp` | Headless CLI tool |

### Files to Modify (existing)

| File | Change |
|---|---|
| `CMakeLists.txt` (root) | Full rewrite — OCIO, NodeEditor, lib/app targets |
| `src/CMakeLists.txt` | Add core/, nodes/, ui/ subdirs |
| `src/pages/Main.qml` | Replace with node editor workspace |
| `src/pages/SplashScreen.qml` | Minor — update app name, theme colors |
| `main.cpp` | Init OCIO, register nodes, set up main window |

---

### Key Patterns

**Node pattern** (following NodeEditor conventions):
```cpp
class ColorSpaceConvertNode : public NodeEditor::BaseNode {
    Q_OBJECT
    QString nodeType() const override { return "mokm/ocio/colorspaceConvert"; }
    QString nodeName() const override { return "ColorSpace Convert"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString displayColor() const override { return "#FACC15"; }  // Theme.primary gold
    QList<PortInfo> inputSpec() const override { return { ... }; }
    QList<PortInfo> outputSpec() const override { return { ... }; }
    QVariantMap compute(const QVariantMap &inputs) override {
        // Get ImageBuffer from input, apply OCIO processor, return
    }
};
```

**Registration** (one function to register all MOKM nodes):
```cpp
void registerMOKMNodeTypes(NodeEditor::GraphModel *model) {
    model->registerCategory({"MOKM", "MOKM Color", QColor("#FACC15")});
    registerNodeType<ColorSpaceConvertNode>(model, "MOKM");
    registerNodeType<DisplayViewNode>(model, "MOKM");
    // ...
}
```

---

### Questions for you before I start implementing:

1. The current CMake has project name `learConnection_2` / target `applearConnection_2` — should I rename everything to `MOKM_Color_Processor` (or keep a different name)?

2. For headless mode — do you want a separate CLI executable (`mokm-cli`) built alongside the GUI, or should both modes be in the same binary with a `--headless` flag?

3. The existing NodeEditor's `BaseNode::compute()` uses `QVariantMap` for data flow. For ImageBuffer passing, I'll need to use `QSharedPointer<ImageBuffer>` wrapped in QVariant. Does this approach work for you, or do you prefer a different data-sharing strategy?

4. For the video pipeline — initial MVP could just do image sequences (frame-by-frame). Do you need actual video container support (e.g., FFmpeg) or is EXR/TIFF sequence sufficient for now?
