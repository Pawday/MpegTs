cmake_minimum_required(VERSION 3.10)

project(MPEG_TS C)

add_subdirectory(mpegts)

option(MPEGTS_BUILD_EXAMPLES "Enable compiling example for mpegts library" ON)
if (MPEGTS_BUILD_EXAMPLES)
    message(STATUS "MPEGTS_BUILD_EXAMPLES is set")
    add_subdirectory(examples)
endif()

option(MPEGTS_ENABLE_TESTING "Enable compiling tests for mpegts library" ON)
if (MPEGTS_ENABLE_TESTING)
    enable_testing()
    message(STATUS "MPEGTS_ENABLE_TESTING is set")
    add_subdirectory(tests)
endif()
