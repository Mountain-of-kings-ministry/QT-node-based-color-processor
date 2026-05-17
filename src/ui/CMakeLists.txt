add_library(MOKMColorGUI
    FileDialogHelper.cpp
    DisplayImageProvider.cpp
    DisplayImageHelper.cpp
    MOKMApplication.cpp
)

target_include_directories(MOKMColorGUI PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MOKMColorGUI PUBLIC
    MOKMColorLib
    MOKMColorNodes
    NodeEditor
    NodeEditorCore
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Qml Qt6::Quick Qt6::QuickControls2
)

target_compile_definitions(MOKMColorGUI PRIVATE MOKM_LIBRARY)
