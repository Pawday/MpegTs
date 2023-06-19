cmake_minimum_required(VERSION 3.10)

project(MPEG_TS C)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_compile_options("-Wall" "-Wextra" "-Werror" "-pedantic")
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC")
    add_compile_options("-Wno-missing-braces") # see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
endif()

add_subdirectory(net)
add_subdirectory(mpegts)

add_subdirectory(tools)
