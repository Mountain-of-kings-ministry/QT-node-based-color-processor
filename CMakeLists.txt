cmake_minimum_required(VERSION 3.20)
project(MOKM_Color_Processor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

option(BUILD_GUI "Build Qt GUI Application" ON)
option(BUILD_HEADLESS "Build headless CLI tool" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Qml Quick QuickControls2 Concurrent)

qt_standard_project_setup(REQUIRES 6.5)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third_parties/install")
find_package(OpenColorIO REQUIRED CONFIG)

add_subdirectory(third_parties/node/NodeEditor)



add_subdirectory(src)

qt_add_executable(appMOKM_Color_Processor
    main.cpp
    src/resources/resources.qrc
)

set_source_files_properties(src/pages/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(appMOKM_Color_Processor
    URI MOKM_Color_Processor
    VERSION 1.0
    QML_FILES
        src/pages/Main.qml
        src/pages/SplashScreen.qml
        src/pages/Theme.qml
        src/pages/DisplayPanel.qml
    RESOURCES src/resources/resources.qrc
)

set_source_files_properties(src/pages/Main.qml PROPERTIES QT_RESOURCE_ALIAS "Main.qml")
set_source_files_properties(src/pages/SplashScreen.qml PROPERTIES QT_RESOURCE_ALIAS "SplashScreen.qml")
set_source_files_properties(src/pages/Theme.qml PROPERTIES QT_RESOURCE_ALIAS "Theme.qml")

target_include_directories(appMOKM_Color_Processor PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_parties/node/NodeEditor/include
    ${CMAKE_SOURCE_DIR}/third_parties/install/include
)

target_link_libraries(appMOKM_Color_Processor PRIVATE
    MOKMColorGUI
    MOKMColorNodes
    MOKMColorLib
    NodeEditor
    NodeEditorCore
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Qml Qt6::Quick Qt6::QuickControls2
)

set_target_properties(appMOKM_Color_Processor PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.mokm.colorprocessor
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(BUILD_HEADLESS)
    add_subdirectory(tools/mokm-cli)
endif()

include(GNUInstallDirs)

# ── Install libraries ──────────────────────────────────────────
install(TARGETS MOKMColorLib MOKMColorNodes MOKMColorGUI NodeEditorCore
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ── Install executable ────────────────────────────────────────
install(TARGETS appMOKM_Color_Processor
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ── Install headers ────────────────────────────────────────────
install(DIRECTORY include/mokm DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/third_parties/node/NodeEditor/include/NodeEditor
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# ── CMake package config for find_package(MOKM) ───────────────
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/MOKMConfig.cmake.in
    ${CMAKE_BINARY_DIR}/MOKMConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MOKM
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)
install(FILES ${CMAKE_BINARY_DIR}/MOKMConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MOKM
)
