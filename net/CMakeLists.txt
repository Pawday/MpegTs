cmake_minimum_required(VERSION 3.10)

add_library(MulticastSocket OBJECT)

target_sources(MulticastSocket PRIVATE MulticastSocket.c)

target_include_directories(MulticastSocket PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
