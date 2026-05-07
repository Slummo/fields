cmake_minimum_required(VERSION 4.0)
project(fields VERSION 1.0.0 LANGUAGES C)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)

set_default_build_type()
set_output_directories()

add_subdirectory(extern)

# Executable
add_executable(${PROJECT_NAME} "")
target_sources(${PROJECT_NAME} PRIVATE
    src/main.c
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_11)
apply_compile_options(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE calyx::lib)