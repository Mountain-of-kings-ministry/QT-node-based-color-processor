**✅ Here is a clean and professional CMake configuration** tailored to your current `third_parties` folder structure.

### Root `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20)
project(MOKM_Color_Processor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==================== Output Directories ====================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ==================== Options ====================
option(BUILD_GUI "Build Qt GUI Application" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# ==================== Find Dependencies ====================

# Qt
find_package(Qt6 COMPONENTS Core Widgets Gui Concurrent REQUIRED)
qt_standard_project_setup()

# OpenColorIO (from your third_parties)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third_parties/OpenColorIO/install")
find_package(OpenColorIO REQUIRED CONFIG)

# ==================== Node Editor (as subdirectory) ====================
add_subdirectory(third_parties/node/NodeEditor)

# ==================== Main Library (Headless - No Qt) ====================
add_library(MOKMColorLib STATIC
    src/core/ImageBuffer.cpp
    src/core/OCIOWrapper.cpp
    src/core/NodeBase.cpp
    src/core/PipelineExecutor.cpp
    src/core/DataTypes.cpp
    # Add more core files here later
)

target_include_directories(MOKMColorLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_parties/OpenColorIO/install/include
)

target_link_libraries(MOKMColorLib PUBLIC
    OpenColorIO::OpenColorIO
)

# ==================== GUI Application ====================
if(BUILD_GUI)
    qt_add_executable(MOKMColorProcessor
        src/main.cpp
        src/ui/MainWindow.cpp
        src/ui/NodeWorkspace.cpp
        src/ui/PreviewWindow.cpp
        src/ui/Inspectors/*.cpp
        # Add your UI files here
    )

    target_link_libraries(MOKMColorProcessor PRIVATE
        MOKMColorLib
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        NodeEditor::NodeEditor   # From the NodeEditor subdirectory
    )

    target_include_directories(MOKMColorProcessor PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
endif()

# ==================== Install ====================
install(TARGETS MOKMColorProcessor RUNTIME DESTINATION bin)
install(TARGETS MOKMColorLib ARCHIVE DESTINATION lib)
```

---

### Recommended Folder Structure (Suggested)

Your current `third_parties` is good. Here’s how your full project should look:

```
MOKM_Color_Processor/
├── CMakeLists.txt                    ← (the one above)
├── third_parties/
│   ├── node/                         ← your Qt Node Editor
│   └── OpenColorIO/
│       └── install/                  ← after you run cmake --install
├── src/
│   ├── core/                         ← Library core (no Qt)
│   ├── nodes/                        ← All node implementations
│   ├── ui/                           ← GUI only files
│   └── main.cpp
├── include/                          ← Public headers if making SDK
└── configs/                          ← Default .ocio files
```

---

### Build OpenColorIO First (Important!)

Before building the main project, make sure OCIO is installed:

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

---

### How to Build Your Project

```bash
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build . --parallel $(nproc)
```

---

Would you like me to also provide:

1. A more detailed `src/CMakeLists.txt` (modular)
2. Specific CMake for the `nodes/` folder
3. `conan.cmake` or `vcpkg` version (optional)

Just say what you need next and I’ll provide it.
