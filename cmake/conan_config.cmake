find_program(conanexecutable "conan")
if(NOT conanexecutable)
    message(WARNING "Tool conan is not installed. Check README.md for build instructions without conan.")
else()
    message(STATUS "Found conan. Installing dependencies.")
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                      "${CMAKE_BINARY_DIR}/conan.cmake")
    endif()
    include(${CMAKE_BINARY_DIR}/conan.cmake)
    conan_cmake_run(CONANFILE conanfile.py
                    BASIC_SETUP CMAKE_TARGETS
                    BUILD missing)
endif()
