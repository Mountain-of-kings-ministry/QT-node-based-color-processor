cd OpenColorIO-2.5.2
mkdir build && cd build

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=install \
  -DOCIO_BUILD_SHARED=ON \
  -DOCIO_BUILD_APPS=ON \
  -DOCIO_INSTALL_EXT_PACKAGES=ALL \
  -DCMAKE_CXX_STANDARD=17

cmake --build . --parallel 8
cmake --install .

find_package(OpenColorIO REQUIRED CONFIG PATHS "C:/path/to/OpenColorIO-2.5.2/install")

target_link_libraries(YourNodeEditorApp PRIVATE OpenColorIO::OpenColorIO)
target_include_directories(YourNodeEditorApp PRIVATE ${OPENCOLORIO_INCLUDE_DIRS})