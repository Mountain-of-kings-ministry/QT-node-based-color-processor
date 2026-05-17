# Node Reference

All MOKM nodes are registered under the **MOKM** category (gold accent `#FACC15`).

## OCIO Nodes

### OCIO Config

Loads an OpenColorIO configuration file.

| Port | Direction | Type | Default |
|---|---|---|---|
| `configPath` | Input | String | `""` |
| `configRef` | Output | String | (path) |

**Parameters:**
- `configPath` — Path to `.ocio` file

When evaluated, loads the config into the global `OCIOWrapper` singleton.  
All subsequent OCIO nodes use this config.

---

### ColorSpace Convert

Converts an image from one color space to another.

| Port | Direction | Type | Default |
|---|---|---|---|
| `image` | Input | Image | — |
| `srcColorSpace` | Input | String | `"scene_linear"` |
| `dstColorSpace` | Input | String | `"output_srgb"` |
| `image` | Output | Image | — |

**Parameters:**
- `srcColorSpace` — Source color space name (from config)
- `dstColorSpace` — Destination color space name (from config)

**Processes:** Creates an OCIO processor from `srcColorSpace → dstColorSpace` and applies it.

---

### Display + View

Applies a display + view transform (the standard way to preview for a monitor).

| Port | Direction | Type | Default |
|---|---|---|---|
| `image` | Input | Image | — |
| `display` | Input | String | `"sRGB"` |
| `view` | Input | String | `"Default"` |
| `inputColorSpace` | Input | String | `"scene_linear"` |
| `image` | Output | Image | — |

**Parameters:**
- `display` — Display device name (from config)
- `view` — View transform name (from config)
- `inputColorSpace` — The color space of the incoming image

**Processes:** Creates `config->getProcessor(inputCS, display, view)` and applies it.

---

### File Transform (LUT)

Loads and applies a LUT file (`.cube`, `.3dl`, `.spi1d`, `.spi3d`, etc.).

| Port | Direction | Type | Default |
|---|---|---|---|
| `image` | Input | Image | — |
| `lutPath` | Input | String | `""` |
| `interpolation` | Input | String | `"linear"` |
| `image` | Output | Image | — |

**Parameters:**
- `lutPath` — Path to LUT file on disk
- `interpolation` — Interpolation method (future)

**Processes:** Creates `OCIO::FileTransform` from the LUT path and applies it.

---

### Look Transform

Applies a named creative look from the OCIO config.

| Port | Direction | Type | Default |
|---|---|---|---|
| `image` | Input | Image | — |
| `look` | Input | String | `""` |
| `srcColorSpace` | Input | String | `"scene_linear"` |
| `dstColorSpace` | Input | String | `"scene_linear"` |
| `image` | Output | Image | — |

**Parameters:**
- `look` — Name of the look (from config's looks list)
- `srcColorSpace` — Source color space for the look
- `dstColorSpace` — Destination color space for the look

**Processes:** Creates `OCIO::LookTransform` and applies it.

---

### Range Transform

Clamps or remaps pixel value ranges.

| Port | Direction | Type | Default |
|---|---|---|---|
| `image` | Input | Image | — |
| `minIn` | Input | Float | `0.0` |
| `maxIn` | Input | Float | `1.0` |
| `minOut` | Input | Float | `0.0` |
| `maxOut` | Input | Float | `1.0` |
| `image` | Output | Image | — |

**Parameters:**
- `minIn` / `maxIn` — Input range
- `minOut` / `maxOut` — Output range

**Processes:** Creates `OCIO::RangeTransform` and applies it.  
Useful for normalizing log data or clamping out-of-range values.

---

## IO Nodes

### Image Loader

Loads an image from disk into a float32 buffer.

| Port | Direction | Type | Default |
|---|---|---|---|
| `path` | Input | String | `""` |
| `colorSpace` | Input | String | `"scene_linear"` |
| `image` | Output | Image | — |

**Parameters:**
- `path` — File path to load
- `colorSpace` — Color space of the loaded image (metadata)

**Supported formats:** PPM (P6 binary). More formats coming.

---

### Image Saver

Saves an image buffer to disk.

| Port | Direction | Type | Default |
|---|---|---|---|
| `image` | Input | Image | — |
| `path` | Input | String | `"output.ppm"` |
| `format` | Input | String | `"ppm"` |
| `image` | Output | Image | — |

**Parameters:**
- `path` — Output file path
- `format` — Output format (future)

**Note:** Passes the image through to output so you can chain further nodes.

---

## Output Nodes

### Preview

Sends an image to the floating preview window.

| Port | Direction | Type | Default |
|---|---|---|---|
| `image` | Input | Image | — |
| `image` | Output | Image | — |

When computed:
1. Converts float32 buffer to ARGB32 `QImage`
2. Clamps values to [0, 1] range
3. Emits signal to update the preview window (zoom/pan preserved)
4. Passes image through on the output port

## Port Types

| Type | Description |
|---|---|
| **Image** | `std::shared_ptr<mokm::ImageBuffer>` — float32 pixel buffer |
| **String** | `QString` — text values (paths, color space names) |
| **Float** | `double` — numeric values |
| **Int** | `int` — integer values |

## Node Colors

| Category | Color | Hex |
|---|---|---|
| MOKM / OCIO | Gold | `#FACC15` |
| MOKM / IO Input | Green | `#22C55E` |
| MOKM / IO Output | Red | `#EF4444` |
| MOKM / Output | Blue | `#3B82F6` |
