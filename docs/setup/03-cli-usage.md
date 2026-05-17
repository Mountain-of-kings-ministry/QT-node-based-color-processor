# CLI Tool — `mokm-cli`

The headless CLI tool processes images and sequences without any GUI.  
Use it for batch rendering, automation, and integration into other pipelines.

## Usage

```
mokm-cli [options]
```

## Options

| Option | Description |
|---|---|
| `-h, --help` | Show help and exit |
| `-v, --version` | Show version and exit |
| `--graph <file>` | Path to node graph JSON file or inline JSON string |
| `--input <path>` | Input image file path |
| `--output <path>` | Output image file path |
| `--sequence <pattern>` | Image sequence pattern (e.g. `frame.%04d.exr`) |
| `--range <start-end>` | Frame range for sequence (e.g. `1-100`) |
| `--config <file>` | Path to `.ocio` config file (overrides graph config) |
| `--list-cs` | List color spaces, displays, and views from a config |

## Examples

### 1. Process a Single Image

```bash
./build/default/bin/mokm-cli \
  --graph my_pipeline.json \
  --input input.exr \
  --output output.exr
```

### 2. List Available Color Spaces

```bash
./build/default/bin/mokm-cli \
  --list-cs \
  --config /usr/share/ocio/aces_1.3/config.ocio
```

Output:

```
Color Spaces:
  scene_linear
  ACEScg
  ACES2065-1
  sRGB
  Rec.709
  ...

Displays:
  sRGB:
    - Default
    - Dolby PQ1000
  DCDM:
    - P3D60
  ...

Looks:
  - ACES 1.0 Saturation
  - Unhighlight
```

### 3. Process an Image Sequence

```bash
./build/default/bin/mokm-cli \
  --graph grade.json \
  --sequence "/render/frame.%04d.exr" \
  --range 1-100 \
  --config config.ocio
```

Input: `frame.0001.exr` … `frame.0100.exr`  
Output: the tool writes processed images back to the same paths (in-place).

### 4. Override OCIO Config

```bash
./build/default/bin/mokm-cli \
  --graph pipeline.json \
  --input shot.exr \
  --output graded.exr \
  --config /show/current/config.ocio
```

Overrides whatever config path is in the graph JSON.

## Graph JSON Format

The `--graph` file describes a node graph in JSON:

```json
{
  "ocioConfig": "/path/to/config.ocio",
  "nodes": [
    {
      "id": "loader1",
      "type": "mokm/io/imageLoader",
      "x": 0, "y": 0,
      "params": { "path": "/input.exr", "colorSpace": "scene_linear" }
    },
    {
      "id": "convert1",
      "type": "mokm/ocio/colorSpaceConvert",
      "x": 200, "y": 0,
      "params": { "srcColorSpace": "scene_linear", "dstColorSpace": "output_srgb" }
    }
  ],
  "edges": [
    {
      "id": "e1",
      "sourceNode": "loader1",
      "sourcePort": "image",
      "targetNode": "convert1",
      "targetPort": "image"
    }
  ]
}
```

### Creating Graph Files

1. Build your pipeline visually in the GUI app
2. Save as JSON (`File → Save`) — this generates the same format
3. Use the saved file with `mokm-cli --graph file.json`

## Exit Codes

| Code | Meaning |
|---|---|
| 0 | Success |
| 1 | Error (bad config, missing input, processing failure) |

## Common Use Cases

### Batch Color Space Conversion

```bash
for f in *.exr; do
  ./build/default/bin/mokm-cli \
    --graph convert_to_srgb.json \
    --input "$f" \
    --output "srgb_$f"
done
```

### Automated Render Pipeline

```bash
#!/bin/bash
# Render all shots for today's dailies
CONFIG="/show/aces_1.3/config.ocio"
GRAPH="/show/pipeline/dailies_grade.json"

for shot in /render/shots/*/; do
  name=$(basename "$shot")
  ./build/default/bin/mokm-cli \
    --graph "$GRAPH" \
    --sequence "${shot}plate.%04d.exr" \
    --range 1001-1100 \
    --config "$CONFIG"
done
```
