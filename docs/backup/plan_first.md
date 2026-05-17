**Perfect.** Here's a clean, well-organized **Node List** tailored for your OpenColorIO + Qt Node Editor project, plus notes on the floating preview window.

---

### **Node Categories & Recommended Nodes**

#### **1. Input Nodes**
- **Image Loader** — Load still image (EXR, TIFF, PNG, JPEG, etc.)
- **Image Sequence Loader** — Load image sequence (with frame range, padding)
- **Solid Color** — Generate constant color (with colorspace)
- **OCIO Config Loader** — Load/select .ocio config file
- **OCIO Context** — Set context variables (shot, sequence, etc.)

#### **2. Output Nodes**
- **Image Saver** — Save image with chosen format + metadata
- **EXR Writer** (specialized, high precision)
- **Display Output** — Final output transform for monitor (sRGB, Rec.709, etc.)

#### **3. Core OCIO Nodes** (Most Important)
- **ColorSpace Convert** — Convert between two color spaces
- **Display + View** — Apply display + view transform (most used for preview)
- **Look Transform** — Apply one or multiple looks
- **File Transform** — Load and apply LUT (.cube, .3dl, .spi1d, etc.)
- **Transform** — Generic transform (Matrix, CDL, Log, Exp, etc.)
- **Range Transform** — Clamp / normalize values
- **Role Input / Role Output** — Use OCIO roles (`scene_linear`, `compositing_log`, etc.)

#### **4. Creative / Grading Nodes**
- **CDL** — ASC CDL (Slope, Offset, Power, Saturation)
- **Grade** — Lift/Gamma/Gain / Offset / Saturation
- **Hue Correct**
- **Curve** — Simple RGB curve editor
- **Tone Mapper** — ACES Tone Mapping, Filmic, etc.
- **Exposure / Contrast / Saturation**

#### **5. Utility Nodes**
- **Switch** — Switch between multiple inputs
- **Merge / Blend** — Blend two images (over, add, multiply, screen, etc.)
- **Split** — Split RGBA channels
- **Join** — Combine channels
- **Crop / ROI**
- **Resize / Scale**
- **Premult / Unpremult**
- **Invert**
- **Clamp**

#### **6. Analysis Nodes**
- **Histogram**
- **Waveform**
- **Vectorscope**
- **Pixel Info** (probe color at position)
- **False Color**
- **Compare** (A/B comparison)

#### **7. Advanced / Special Nodes**
- **LUT Baker** — Bake current pipeline into a 3D LUT
- **Processor Cache** — Cache expensive processor
- **Group / Subgraph** — Compound node (very important for organization)
- **Python Script Node** (optional later)
- **External Command** — Call external tool

#### **8. Preview / Output Nodes**
- **Preview** — Send image to floating preview window (this will be your main viewer)
- **Thumbnail** — Small inline preview (lightweight)

---

### **Priority Order (Build This First)**

**Phase 1 (MVP)**
1. Image Loader
2. ColorSpace Convert
3. Display + View
4. Preview (floating window)
5. Image Saver
6. OCIO Config Loader

**Phase 2**
- CDL, Grade, File Transform, Look Transform
- Merge/Blend, Switch

**Phase 3**
- Analysis nodes, Group nodes, Utilities

---

### **Floating Preview Window Plan**

Since you want a **frameless floating window** that stays inside the main app:

**Architecture Suggestion:**

- Create a class `PreviewWindow : public QWidget`
- Set window flags:
  ```cpp
  setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
  ```
- Make it a child of MainWindow (or use parent-child relationship) so it can't be dragged outside.
- Or use `QDockWidget` in a clever way with custom titlebar.

**Recommended Features for PreviewWindow:**
- Real-time update when connected "Preview" node changes
- Multiple viewer support (A, B, A/B wipe, difference)
- OCIO Display + View selector directly in the window
- Zoom / Pan / Fit
- Pixel probe (color picker)
- Frame number / Timecode (if doing video later)
- Exposure slider (temporary grade on preview only)

**Node → Preview Communication:**
- The **Preview** node should **not** render on the node canvas.
- Instead, it should emit a signal like `imageReady(QImage preview, QString colorspace)` to the PreviewWindow.
- Use `std::shared_ptr<ImageBuffer>` for efficient data passing.

Would you like me to write the basic code structure for:
1. `PreviewNode` class
2. `PreviewWindow` class (frameless)

---
