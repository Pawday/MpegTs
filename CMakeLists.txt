cmake_minimum_required(VERSION 3.10)

project(MPEG_TS C)


add_subdirectory(mpegts)


option(MPEGTS_ENABLE_TESTING "Enable compiling tests for mpegts library" ON)
if (MPEGTS_ENABLE_TESTING)
    message(STATUS "MPEGTS_ENABLE_TESTING is set")
    add_subdirectory(tests)
endif()
