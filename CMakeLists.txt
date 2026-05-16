cmake_minimum_required(VERSION 3.16)

project(learConnection_2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(applearConnection_2
    main.cpp
    src/resources/resources.qrc
)

add_subdirectory(src)

set_source_files_properties(src/pages/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

# Automatically find all cpp and h files in src/functions
file(GLOB_RECURSE FUNCTION_SOURCES "src/functions/*.cpp" "src/functions/*.h")

# Automatically find all subdirectories in src/functions to add to include path
file(GLOB FUNCTION_SUBDIRS "src/functions/*")
foreach(dir ${FUNCTION_SUBDIRS})
    if(IS_DIRECTORY ${dir})
        list(APPEND FUNCTION_INCLUDE_DIRS ${dir})
    endif()
endforeach()

qt_add_qml_module(applearConnection_2
    URI learConnection_2
    QML_FILES
        src/pages/Main.qml
        src/pages/Theme.qml
        src/pages/SplashScreen.qml
        SOURCES ${FUNCTION_SOURCES}
        RESOURCES src/resources/resources.qrc
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(applearConnection_2 PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.applearConnection_2
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(applearConnection_2
    PRIVATE Qt6::Quick
)

if(FUNCTION_INCLUDE_DIRS)
    target_include_directories(applearConnection_2 PRIVATE ${FUNCTION_INCLUDE_DIRS})
endif()

install(TARGETS applearConnection_2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
