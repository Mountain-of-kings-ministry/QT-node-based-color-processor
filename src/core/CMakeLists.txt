add_library(MOKMColorLib
    ImageBuffer.cpp
    OCIOWrapper.cpp
    PipelineExecutor.cpp
    NodeGraph.cpp
)

target_include_directories(MOKMColorLib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MOKMColorLib PUBLIC
    Qt6::Core
    OpenColorIO::OpenColorIO
)

target_include_directories(MOKMColorLib SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/third_parties/install/include
)

target_compile_definitions(MOKMColorLib PRIVATE MOKM_LIBRARY)
