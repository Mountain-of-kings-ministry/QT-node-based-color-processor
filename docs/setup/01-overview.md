# MOKM Color Processor — Overview

## What Is It?

MOKM Color Processor is a **node-based color management system** powered by **OpenColorIO (OCIO)**.  
It works with both **still images** and **video sequences** and is designed to be:

- A **standalone desktop application** (Qt GUI with node editor)
- A **C++ library** you can embed into other tools and pipelines
- A **headless CLI tool** for batch / automated rendering

## Architecture

```
┌─────────────────────────────────────────────────┐
│              Host / External App                │
├─────────────────────────────────────────────────┤
│                MOKM Color Lib                   │
│   (Core + Nodes + Pipeline — no Qt GUI)        │
├─────────────────────────────────────────────────┤
│            Node Graph System                    │
│   (GraphModel, DataFlowEngine, BaseNode)        │
├─────────────────────────────────────────────────┤
│         OCIO + Processing Engine                │
│   (OCIOWrapper, ImageBuffer, CPUProcessor)      │
├─────────────────────────────────────────────────┤
│           Qt GUI (Optional)                     │
│   (NodeEditorWorkspace, PreviewWindow)          │
└─────────────────────────────────────────────────┘
```

### Layers

| Layer | Name | Qt? | Purpose |
|---|---|---|---|
| 0 | Host / External App | No | Your renderer, DCC, or pipeline |
| 1 | **MOKMColorLib** | No | `ImageBuffer` + `OCIOWrapper` + `PipelineExecutor` |
| 2 | **MOKMColorNodes** | Core only | All node implementations (OCIO, IO, grading) |
| 3 | **MOKMColorGUI** | Yes | `PreviewWindow` + `MOKMApplication` integration API |
| 4 | **appMOKM_Color_Processor** | Yes | Qt executable (thin wrapper around MOKMApplication) |

## Core Components

### ImageBuffer (`include/mokm/ImageBuffer.h`)
Float32 pixel buffer with metadata (width, height, channels, color space).  
Supports: load/save PPM, ROI extraction, clone.

### OCIOWrapper (`include/mokm/OCIOWrapper.h`)
Singleton that wraps all OCIO interactions:
- Load `.ocio` configs
- Create and cache `Processor` objects
- `apply()` transforms onto `ImageBuffer`
- Query color spaces, displays, views, looks

### PipelineExecutor (`include/mokm/PipelineExecutor.h`)
Headless graph evaluation engine:
- Load a `GraphDesc` (serializable JSON)
- Topological sort and lazy evaluation
- Process single images or sequences
- Zero Qt dependency

### NodeGraph (`include/mokm/NodeGraph.h`)
Serializable graph description:
- Nodes (type, position, parameters)
- Edges (source/target connections)
- Serialize to/from JSON

## Three Ways to Use

| Mode | What You Get | When to Use |
|---|---|---|---|
| **GUI App** | Node editor workspace + floating preview + inspectors | Interactive color grading, look development |
| **C++ Library** | Link `MOKMColorLib`, `MOKMColorNodes`, or `MOKMColorGUI` (shared) | Integrate into Nuke, Hiero, or custom pipeline ([integration guide](./05-integration.md)) |
| **CLI Tool** | `mokm-cli --graph graph.json --input in.exr --output out.exr` | Batch rendering, automation, CI/CD |

## Theme

The app uses a professional dark theme defined in `src/pages/Theme.qml` (QML singleton):
- Deep navy-black background (`#0B1120`)
- Rich gold primary accent (`#FACC15`)
- All UI components (sidebars, buttons, panels) use the theme
- No additional theming work needed — just reference `Theme.background`, `Theme.primary`, etc.
