add_executable(mokm-cli
    main.cpp
)

target_link_libraries(mokm-cli PRIVATE
    MOKMColorLib
    Qt6::Core
)

target_include_directories(mokm-cli PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_parties/install/include
)

install(TARGETS mokm-cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
