# Developer Guide — Extending the System

## Project Structure

```
MOKM_Color_Processor/
├── CMakeLists.txt                    # Root build config
├── CMakePresets.json                 # Build presets (Qt 6.11)
├── main.cpp                          # App entry point (uses MOKMApplication)
├── include/
│   └── mokm/                         # Public API headers
│       ├── Export.h                  # MOKM_EXPORT macro for shared libs
│       ├── MOKMApplication.h         # Integration entry point
│       ├── ImageBuffer.h
│       ├── OCIOWrapper.h
│       ├── PipelineExecutor.h
│       ├── NodeGraph.h
│       └── MOKMVersion.h
├── src/
│   ├── core/                         # MOKMColorLib (shared)
│   │   ├── ImageBuffer.cpp
│   │   ├── OCIOWrapper.cpp
│   │   ├── PipelineExecutor.cpp
│   │   └── NodeGraph.cpp
│   ├── nodes/                        # MOKMColorNodes (shared)
│   │   ├── registerMOKMNodes.h/.cpp
│   │   ├── ocio/
│   │   ├── io/
│   │   └── output/
│   ├── ui/                           # MOKMColorGUI (shared)
│   │   ├── MOKMApplication.cpp
│   │   ├── PreviewWindow.h/.cpp
│   │   └── CMakeLists.txt
│   ├── pages/                        # QML pages (embedded in resources)
│   │   ├── Main.qml
│   │   ├── SplashScreen.qml
│   │   └── Theme.qml
│   ├── functions/
│   ├── providers/
│   └── resources/
├── third_parties/
│   ├── OpenColorIO/
│   ├── install/                      # OCIO installed headers + libs
│   └── node/                         # Qt Node Editor (submodule)
├── tools/
│   └── mokm-cli/
└── docs/
    └── setup/
```

## Entry Point

The application entry point is minimal — it delegates to `MOKMApplication`:

```cpp
// main.cpp
#include <mokm/MOKMApplication.h>

int main(int argc, char *argv[]) {
    mokm::MOKMApplication app(argc, argv);
    return app.exec();
}
```

`MOKMApplication` handles:
- Creating `QApplication` and setting the style
- Building a `GraphModel` and registering all default + MOKM node types
- Creating the floating `PreviewWindow`
- Setting up the `QQmlApplicationEngine` (import paths, context properties)
- Registering C++ types (`GraphModel`, `DataFlowEngine`, `UndoManager`) with QML

## How to Add a New Node

### 1. Create the Node Header

Create `src/nodes/<category>/MyNode.h`:

```cpp
#pragma once

#include <NodeEditor/BaseNode.h>
#include "mokm/ImageBuffer.h"
#include "mokm/OCIOWrapper.h"

class MyNode : public NodeEditor::BaseNode {
    Q_OBJECT
public:
    QString nodeType() const override { return "mokm/<category>/myNode"; }
    QString nodeName() const override { return "My Node"; }
    QString nodeCategory() const override { return "MOKM"; }
    QString nodeSubCategory() const override { return "<Category>"; }
    QString displayColor() const override { return "#<HEX>"; }

    QList<NodeEditor::PortInfo> inputSpec() const override {
        return {
            {NodeEditor::PortType::Image, "image", QVariant()},
            {NodeEditor::PortType::Float, "amount", QVariant(1.0)}
        };
    }

    QList<NodeEditor::PortInfo> outputSpec() const override {
        return {{NodeEditor::PortType::Image, "image", QVariant()}};
    }

    QVariantMap compute(const QVariantMap &inputs) override {
        QVariantMap out;
        auto imgVar = inputs.value("image");
        if (!imgVar.isValid()) return out;

        auto buf = imgVar.value<mokm::ImageBufferPtr>();
        if (!buf) return out;

        // ... process buf ...

        out["image"] = imgVar;
        return out;
    }
};
```

### 2. Create the CPP File

Create `src/nodes/<category>/MyNode.cpp`:

```cpp
#include "MyNode.h"
```

(Needed for AUTOMOC to process `Q_OBJECT`.)

### 3. Register the Node

In `src/nodes/registerMOKMNodes.cpp`:

```cpp
#include "<category>/MyNode.h"

void registerMOKMNodeTypes(NodeEditor::GraphModel *model)
{
    // ... existing registrations ...
    using NodeEditor::registerNodeType;
    registerNodeType<MyNode>(model, "MOKM");
}
```

### 4. Add to CMakeLists

In `src/nodes/CMakeLists.txt`:

```cmake
add_library(MOKMColorNodes
    # ... existing files ...
    <category>/MyNode.cpp
)
```

(No `STATIC` keyword — library type follows `BUILD_SHARED_LIBS`.)

### 5. Build and Test

```bash
cmake --build --preset default
```

Your node appears under **MOKM → <Category>** in the palette.

## How OCIO Processing Works

1. **Config Loading**: `OCIOWrapper::loadConfig()` loads `.ocio` → stores `ConstConfigRcPtr`
2. **Processor Creation**: `OCIOWrapper::getProcessor(src, dst)` calls `config->getProcessor()` → cached
3. **Processing**: `OCIOWrapper::apply(processor, buffer)`:
   - Gets `CPUProcessor` from the processor
   - Wraps `ImageBuffer` data in `OCIO::PackedImageDesc`
   - Calls `CPUProcessor::apply()`

Processors are cached by `(src, dst, context)` and cleared when a new config is loaded.

## How the Node Editor Evaluation Works

1. **GraphModel** stores nodes and edges
2. **DataFlowEngine** monitors changes and triggers evaluation
3. On each change:
   - Marks dirty nodes
   - Topologically sorts
   - Calls `compute()` on each dirty node
   - Passes QVariantMap from connected outputs

Images are passed as `std::shared_ptr<mokm::ImageBuffer>` wrapped in `QVariant`.  
Extract with `imgVar.value<mokm::ImageBufferPtr>()`.

## Theming

`Theme.qml` singleton (`src/pages/Theme.qml`):

```qml
Theme.background     // #0B1120  — main background
Theme.primary        // #FACC15  — gold accent
Theme.surface        // #0F172A  — card/panel background
Theme.foreground     // #F8FAFC  — primary text
Theme.border         // #243041  — borders
```

Use in QML: `color: Theme.background`

## Building for Other Platforms

### macOS

```bash
brew install qt@6
cmake --preset default -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
cmake --build --preset default
```

### Windows

```powershell
cmake --preset default -DCMAKE_PREFIX_PATH=C:/Qt/6.11.0/msvc2022_64
cmake --build --preset default
```

## Common Issues

### "module NodeEditor is not installed"

This is resolved at the C++ level in `MOKMApplication.cpp`:
- The NodeEditor QML module import path is added from the build tree
- C++ types (`GraphModel`, `DataFlowEngine`, `UndoManager`) are registered with QML
- No additional import path setup needed at runtime

### "Segfault when loading OCIO config"

Make sure the config file exists and is valid OCIO YAML:

```bash
ociocheck /path/to/config.ocio
```

### "Nodes don't appear in palette"

Verify:
1. `MOKMApplication` constructor calls `registerMOKMNodeTypes()` automatically
2. The registry model is passed to the QML context as `_nodeTypeRegistry`
3. `NodeEditorWorkspace` copies registrations via `qmlCopyRegistryFrom()`
