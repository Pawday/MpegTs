cmake_minimum_required(VERSION 3.10)

project(MPEG_TS C)


add_compile_options("-Wall" "-Wextra" "-Werror" "-pedantic")

add_subdirectory(net)
add_subdirectory(mpegts)


add_executable(StreamDumper StreamDumper.c)

target_link_libraries(StreamDumper PRIVATE MulticastSocket)
