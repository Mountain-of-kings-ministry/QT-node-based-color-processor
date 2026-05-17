# Setup & Build Guide

## Prerequisites

- **Linux** (Ubuntu 22.04+ / Debian 12+)
- **C++17 compiler** (GCC 11+ or Clang 14+)
- **CMake** ≥ 3.20
- **Ninja** build system
- **Qt 6.5+** (Qt 6.11 recommended)
- **Git**

## 1. Install System Dependencies

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    libgl1-mesa-dev \
    libxkbcommon-dev \
    patchelf
```

## 2. Install Qt 6.11

Download and install Qt from [qt.io](https://www.qt.io/download-qt-installer)  
or use the offline installer. Install to `~/Qt/6.11.0/gcc_64`.

Verify:

```bash
~/Qt/6.11.0/gcc_64/bin/qmake6 --version
# Output: QMake version 3.1, Using Qt version 6.11.0
```

## 3. Clone the Repository

```bash
git clone <repo-url> MOKM_Color_Processor
cd MOKM_Color_Processor
```

## 4. Initialize Submodules

The Qt Node Editor is included as a git submodule:

```bash
git submodule update --init --recursive
```

## 5. Build OpenColorIO (First Time Only)

OCIO v2.5.2 source is in `third_parties/OpenColorIO/`.  
Build it once:

```bash
cd third_parties/OpenColorIO

mkdir -p build && cd build

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=../install \
  -DOCIO_BUILD_SHARED=ON \
  -DOCIO_BUILD_APPS=ON \
  -DOCIO_INSTALL_EXT_PACKAGES=ALL \
  -DOCIO_BUILD_PYTHON=OFF \
  -DCMAKE_CXX_STANDARD=17

cmake --build . --parallel $(nproc)
cmake --install .
```

After this, OCIO is installed to `third_parties/install/` with:
- `lib/libOpenColorIO.so`
- `include/OpenColorIO/`
- `bin/` (CLI tools like `ocioconvert`, `ociobakelut`)

## 6. Build the Project

Use the CMake presets (they point to Qt 6.11):

```bash
# Configure (release build)
cmake --preset default

# Build
cmake --build --preset default
```

Output goes to `build/default/bin/`:
- `appMOKM_Color_Processor` — the GUI application
- `mokm-cli` — headless CLI tool

### Build Options

| Option | Default | Description |
|---|---|---|
| `BUILD_GUI` | ON | Build the Qt GUI application |
| `BUILD_HEADLESS` | ON | Build the CLI tool |
| `BUILD_SHARED_LIBS` | ON | Build shared libraries |

```bash
# Example: disable GUI, build only CLI and library
cmake --preset default -DBUILD_GUI=OFF
cmake --build --preset default
```

### Debug Build

```bash
cmake --preset debug
cmake --build --preset debug
```

## 7. Run the App

### GUI Application

```bash
./build/default/bin/appMOKM_Color_Processor
```

### CLI Tool

```bash
./build/default/bin/mokm-cli --help
```

## 8. Verify OCIO Works

Test that OCIO is properly linked:

```bash
./build/default/bin/mokm-cli --list-cs --config /path/to/config.ocio
```

This lists all color spaces, displays, and views from your OCIO config.

## Troubleshooting

### "Qt not found"

Set `CMAKE_PREFIX_PATH` manually:

```bash
cmake --preset default -DCMAKE_PREFIX_PATH=~/Qt/6.11.0/gcc_64
```

### "OpenColorIO not found"

Make sure you built and installed OCIO in step 5.  
The `find_package` looks in `third_parties/install/`.

### Linker errors

Run `ldd build/default/bin/appMOKM_Color_Processor | grep "not found"` to find missing libraries.  
Common missing: `libOpenColorIO.so` — set `LD_LIBRARY_PATH`:

```bash
export LD_LIBRARY_PATH=$PWD/third_parties/install/lib:$LD_LIBRARY_PATH
```
