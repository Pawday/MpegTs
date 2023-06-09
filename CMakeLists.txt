cmake_minimum_required(VERSION 3.10)

project(MPEG_TS C)


add_compile_options("-Wall" "-Wextra" "-Werror" "-pedantic")

add_subdirectory(net)
add_subdirectory(mpegts)

add_subdirectory(tools)
