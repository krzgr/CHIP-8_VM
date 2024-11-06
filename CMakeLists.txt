cmake_minimum_required(VERSION 3.10)
project("CHIP-8_VM")

add_subdirectory(external/SFML)

if(NOT TARGET sfml-graphics)
	message(FATAL_ERROR "sfml-graphics is required!")
elseif(NOT TARGET sfml-main AND WIN32)
	message(FATAL_ERROR "sfml-main is required!")
elseif(NOT TARGET sfml-system)
	message(FATAL_ERROR "sfml-system is required!")
elseif(NOT TARGET sfml-window)
	message(FATAL_ERROR "sfml-window is required!")
endif()

if(NOT BUILD_SHARED_LIBS)
	add_compile_definitions(SFML_STATIC)
endif()

set(SRC_FILES
    "src/main.cpp"
    "src/CHIP8.hpp"
    "src/CHIP8.cpp"
    "src/CHIP8_Mediator.hpp"
    "src/CHIP8_Mediator.cpp"
    "src/CHIP8_GUI.hpp"
    "src/CHIP8_GUI.cpp"
)

add_executable(${PROJECT_NAME}  ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE "external/SFML/include")

target_link_libraries(${PROJECT_NAME} "sfml-graphics;sfml-system;sfml-window")

if(WIN32)
    target_link_libraries(${PROJECT_NAME} "sfml-main")
endif()


set_target_properties( ${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

add_subdirectory(tests)
