cmake_minimum_required(VERSION 4.0)
project(fields VERSION 1.0.0 LANGUAGES C)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_SELF TRUE)
else()
    set(IS_SELF FALSE)
endif()

if (IS_SELF)
    # Strip the project source directory from __FILE__ macros
    if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
      add_compile_options(-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=)
    endif()

    add_compile_options(
        -Wall
        -Wextra
        -Werror
        -Wsign-conversion
        -std=c99

        # Debug flags
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-fsanitize=address>
        $<$<CONFIG:Debug>:-fsanitize=leak>
        $<$<CONFIG:Debug>:-fsanitize=undefined>
        $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
        $<$<CONFIG:Debug>:-DDEBUG>

        # Release flags
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-DNDEBUG>
    )

    add_link_options(
        $<$<CONFIG:Debug>:-fsanitize=address>
        $<$<CONFIG:Debug>:-fsanitize=leak>
        $<$<CONFIG:Debug>:-fsanitize=undefined>
    )

    # Default to debug build
    if(NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE Debug)
    endif()
endif()

add_subdirectory(extern)

add_executable(main src/main.c)
target_link_libraries(main PRIVATE calyx::lib)