cmake_minimum_required(VERSION 3.10)
project("CHIP-8_VM")

set(SRC_FILES
    "src/main.cpp"
    "src/CHIP8.hpp"
    "src/CHIP8.cpp"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties( ${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

add_subdirectory(tests)