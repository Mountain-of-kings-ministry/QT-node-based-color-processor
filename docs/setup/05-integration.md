# Integration Guide — Using MOKM as a C++ Library

You can embed MOKM's color processing engine into your own application.  
Libraries are built as **shared** by default (`BUILD_SHARED_LIBS=ON`).

## Library Overview

| Library | Qt Dep? | Contents |
|---|---|---|
| `MOKMColorLib` | `Qt6::Core` | `ImageBuffer`, `OCIOWrapper`, `PipelineExecutor`, `NodeGraph` |
| `MOKMColorNodes` | `Qt6::Core` + `Qt6::Gui` | All OCIO/IO node implementations + registration function |
| `MOKMColorGUI` | Qt Widgets + Qml + Quick | `PreviewWindow`, `MOKMApplication` — the full GUI |

For headless integration: link only **`MOKMColorLib`** or **`MOKMColorNodes`**.

## CMake Integration

### Via `find_package` (Recommended)

After installing MOKM, or pointing at the build tree:

```cmake
find_package(MOKM REQUIRED)

# Headless — core processing only
target_link_libraries(myapp PRIVATE MOKM::MOKMColorLib)

# With node implementations
target_link_libraries(myapp PRIVATE MOKM::MOKMColorNodes)

# Full GUI embedding
target_link_libraries(myapp PRIVATE MOKM::MOKMColorGUI)
```

The config file (installed to `<prefix>/lib/cmake/MOKM/MOKMConfig.cmake`) automatically
finds Qt6 and OpenColorIO, and provides all three IMPORTED targets.

### Via `add_subdirectory`

```cmake
add_subdirectory(path/to/MOKM_Color_Processor)

target_link_libraries(myapp PRIVATE
    MOKMColorLib
    # MOKMColorNodes
    # MOKMColorGUI
)
target_include_directories(myapp PRIVATE
    ${CMAKE_SOURCE_DIR}/path/to/MOKM_Color_Processor/include
)
```

## Embedding the Full GUI

The `MOKMApplication` class wraps all initialization — `QApplication`,
`GraphModel`, type registration, QML engine, import paths, and preview window —
into a single entry point.

```cpp
#include <mokm/MOKMApplication.h>

int main(int argc, char *argv[]) {
    mokm::MOKMApplication app(argc, argv);
    return app.exec();
}
```

### Accessing Internals

```cpp
// Get the main QML window (for embedding via createWindowContainer)
QWindow *win = app.mainWindow();

// Access the node type registry
NodeEditor::GraphModel *registry = app.registryModel();

// Access the floating preview widget
PreviewWindow *preview = app.previewWindow();

// Access the QML engine (for setting custom context properties)
QQmlApplicationEngine *engine = app.engine();
```

## API Reference

### ImageBuffer

```cpp
#include <mokm/ImageBuffer.h>

mokm::ImageBuffer buf;

// Load from PPM file
buf.loadFromFile("input.ppm");

// Or allocate directly
mokm::ImageBuffer buf(1920, 1080, 4, mokm::PixelFormat::F32);

// Access pixel data
float* pixels = buf.data();
int w = buf.width();
int h = buf.height();
int c = buf.channels();

buf.setColorSpace("scene_linear");
std::string cs = buf.colorSpace();

auto copy = buf.clone();
auto region = buf.roi(100, 100, 200, 200);
buf.saveToFile("output.ppm");
```

### OCIOWrapper

```cpp
#include <mokm/OCIOWrapper.h>

auto& ocio = mokm::OCIOWrapper::instance();

ocio.loadConfig("/path/to/config.ocio");

auto proc = ocio.getProcessor("scene_linear", "output_srgb");

mokm::ImageBuffer img;
img.loadFromFile("input.exr");
img.setColorSpace("scene_linear");
ocio.apply(proc, img);

auto displayProc = ocio.getDisplayProcessor("sRGB", "Default", "scene_linear");
ocio.apply(displayProc, img);

auto spaces = ocio.colorSpaces();
auto displays = ocio.displays();
auto views = ocio.views("sRGB");
auto looks = ocio.looks();
```

### PipelineExecutor (Headless Graph)

```cpp
#include <mokm/PipelineExecutor.h>

mokm::PipelineExecutor executor;

std::string json = R"({
    "ocioConfig": "/path/to/config.ocio",
    "nodes": [{
        "id": "convert",
        "type": "mokm/ocio/colorSpaceConvert",
        "params": {
            "srcColorSpace": "scene_linear",
            "dstColorSpace": "output_srgb"
        }
    }],
    "edges": []
})";
executor.loadGraphFromJson(json);

mokm::ImageBuffer img;
img.loadFromFile("input.exr");
img.setColorSpace("scene_linear");
executor.processImage(img);
img.saveToFile("output.exr");
```

### GraphDesc (Manual Graph Construction)

```cpp
#include <mokm/NodeGraph.h>

mokm::GraphDesc graph;
graph.ocioConfigPath = "/path/to/config.ocio";

mokm::NodeDesc loader;
loader.id = "loader1";
loader.type = "mokm/io/imageLoader";
loader.params["path"] = "/input.exr";
loader.params["colorSpace"] = "scene_linear";

mokm::NodeDesc convert;
convert.id = "convert1";
convert.type = "mokm/ocio/colorSpaceConvert";
convert.params["srcColorSpace"] = "scene_linear";
convert.params["dstColorSpace"] = "output_srgb";

mokm::EdgeDesc edge;
edge.id = "e1";
edge.sourceNode = "loader1";
edge.sourcePort = "image";
edge.targetNode = "convert1";
edge.targetPort = "image";

graph.nodes = {loader, convert};
graph.edges = {edge};

std::string json = graph.serializeToJson();
std::ofstream("pipeline.json") << json;
```

## Embedding in DCC Tools

### In Nuke / Hiero (Python + C++)

```
1. Build MOKM (shared libs)
2. Link or dlopen libMOKMColorLib.so
3. Call OCIOWrapper::apply() on pixel data
```

### In Blender

```
1. Build MOKM (shared libs)
2. Use Python ctypes to call processing functions
3. Convert Blender's ImBuf to ImageBuffer via raw pixel data
```

### In a Custom Renderer

```cpp
#include <mokm/OCIOWrapper.h>
#include <mokm/ImageBuffer.h>

void applyColorPipeline(float* rgba, int w, int h) {
    mokm::ImageBuffer buf;
    buf.setColorSpace("scene_linear");

    auto& ocio = mokm::OCIOWrapper::instance();
    auto proc = ocio.getDisplayProcessor("sRGB", "Default", "scene_linear");
    ocio.apply(proc, buf);
}
```

## Exporting for Other Languages

The core library (`MOKMColorLib`) has minimal dependencies (`Qt6::Core` + OCIO).  
You can create bindings for:

- **Python** — via `pybind11` or `cppyy`
- **JavaScript** — via `Node.js` N-API
- **Lua** — via `sol2`
