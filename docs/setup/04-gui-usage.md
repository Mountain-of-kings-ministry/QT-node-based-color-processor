# GUI Application — `appMOKM_Color_Processor`

The GUI provides a visual node editor workspace for building color pipelines interactively.

## Launching

```bash
./build/default/bin/appMOKM_Color_Processor
```

You'll see:
1. **Splash screen** (1.4 second) with MOKM branding
2. **Main window** with the node editor workspace

## Interface Layout

```
┌─────────┬──────────────────────────────────────┬─────────────┐
│ Toolbar │                                      │ Properties  │
│  48px   │        Node Editor Canvas            │   Panel     │
│         │                                      │             │
│  📁     │    [Image Loader]──→[CS Convert]     │  selected   │
│  ➕     │         │               │            │  node props │
│  ▶️     │         └───────→[Preview]            │             │
│         │                                      │             │
│  ⚙️     │                                      │             │
└─────────┴──────────────────────────────────────┴─────────────┘
```

### Left Toolbar

| Button | Shortcut | Action |
|---|---|---|
| Folder | — | Open OCIO config file dialog |
| + (Add) | `Shift+A` | Open node palette to add nodes |
| Play | — | Process all nodes (recompute) |
| Gear | — | Settings (future) |

### Top Bar (File Menu)

- **File → New Tab** (`Ctrl+N`): New blank workspace tab
- **File → Open** (`Ctrl+O`): Open saved `.json` graph
- **File → Save** (`Ctrl+S`): Save current graph
- **File → Save As** (`Ctrl+Shift+S`): Save to new file
- **File → Save All**: Save all open tabs
- **File → Clear All**: Reset current canvas

### Right Properties Panel

When a node is selected, this panel shows its editable parameters:
- Color space dropdowns (populated from loaded OCIO config)
- File path selectors
- Float sliders for grades and transforms

## Building a Pipeline

### Step 1: Add an Image Loader
1. Press `Shift+A` to open the node palette
2. Find **Image Loader** under **MOKM → IO**
3. Click to add it to the canvas
4. Select the node, enter a file path in the Properties panel

### Step 2: Add a ColorSpace Convert
1. `Shift+A` → **MOKM → OCIO → ColorSpace Convert**
2. Connect the **image** port from Image Loader to ColorSpace Convert
3. Set `srcColorSpace` (e.g. `scene_linear`) and `dstColorSpace` (e.g. `output_srgb`)

### Step 3: Add a Preview Node
1. `Shift+A` → **MOKM → Output → Preview**
2. Connect ColorSpace Convert's **image** output to Preview's **image** input
3. The floating preview window opens showing the result
4. Use zoom (scroll wheel) and pan (drag) to inspect pixels

### Step 4: Add a Display + View Transform
1. Add **Display + View** node
2. Connect it between ColorSpace Convert and Preview
3. Select display (e.g. `sRGB`) and view (e.g. `Default`)

## Floating Preview Window

The preview window:
- Is **frameless** and stays inside the main window
- Has a toolbar at the top with:
  - **Display** dropdown (OCIO display selector)
  - **View** dropdown (OCIO view selector)
  - **Exposure** slider (±5 stops temporary exposure adjustment)
- Supports **zoom** (mouse wheel) and **pan** (click and drag)

## Available Nodes

All MOKM nodes are under the **MOKM** category in the palette:

### OCIO Nodes (gold)
| Node | What It Does |
|---|---|
| OCIO Config | Load an `.ocio` config file |
| ColorSpace Convert | Convert between two color spaces |
| Display + View | Apply display + view transform |
| File Transform (LUT) | Load and apply a LUT (.cube, .3dl, .spi1d) |
| Look Transform | Apply a named creative look |
| Range Transform | Clamp / normalize pixel values |

### IO Nodes (green / red)
| Node | What It Does |
|---|---|
| Image Loader | Load an image from disk |
| Image Saver | Save an image to disk |

### Output Nodes (blue)
| Node | What It Does |
|---|---|
| Preview | Send image to floating preview window |

## Keyboard Shortcuts

| Shortcut | Action |
|---|---|
| `Shift+A` | Add node (open palette) |
| `Delete` | Delete selected node |
| `Ctrl+D` / `Shift+D` | Duplicate selected node |
| `Ctrl+N` | New tab |
| `Ctrl+O` | Open graph file |
| `Ctrl+S` | Save graph |
| `Ctrl+Shift+S` | Save as |
| `Ctrl+W` | Close current tab |
| `Ctrl+Tab` | Switch to next tab |
| `Ctrl+Z` | Undo |
| `Ctrl+Shift+Z` / `Ctrl+Y` | Redo |
| `F` | Fit all nodes to view |
| `Shift+V` | Fit view to selected node |

## Working with OCIO Configs

1. Click the **Folder** icon in the left toolbar
2. Select your `.ocio` file
3. The config's color spaces, displays, and views become available in node properties
4. Node dropdowns update dynamically to show available options

## Saving and Loading

Graphs are saved as `.json` files that store:
- All nodes with their type, position, and parameter values
- All edge connections
- The OCIO config path

These files are compatible with the CLI tool (`mokm-cli --graph ...`).
