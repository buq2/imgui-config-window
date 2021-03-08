project(imgui-config-window)
cmake_minimum_required(VERSION 3.1)

include(cmake/conan_config.cmake)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB HEADER_FILES include/*.h )

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME}
    CONAN_PKG::imgui
    CONAN_PKG::sdl2
    CONAN_PKG::glew
    CONAN_PKG::protobuf
    )

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
install(FILES ${HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

OPTION(BUILD_TEST_PACKAGE "Build test package" OFF)
IF(BUILD_TEST_PACKAGE)
    add_subdirectory(test_package)
ENDIF()