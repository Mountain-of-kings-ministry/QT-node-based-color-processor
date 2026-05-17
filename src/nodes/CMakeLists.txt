add_library(MOKMColorNodes
    registerMOKMNodes.cpp
    ocio/ConfigLoaderNode.cpp
    ocio/ColorSpaceConvertNode.cpp
    ocio/DisplayViewNode.cpp
    ocio/FileTransformNode.cpp
    ocio/LookTransformNode.cpp
    ocio/RangeTransformNode.cpp
    io/ImageLoaderNode.cpp
    io/ImageSaverNode.cpp
    output/PreviewNode.cpp
)

target_include_directories(MOKMColorNodes PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_parties/node/NodeEditor/include
)

target_link_libraries(MOKMColorNodes PUBLIC
    MOKMColorLib
    NodeEditorCore
    Qt6::Core Qt6::Gui
)

target_include_directories(MOKMColorNodes SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/third_parties/install/include
)

target_compile_definitions(MOKMColorNodes PRIVATE MOKM_LIBRARY)
