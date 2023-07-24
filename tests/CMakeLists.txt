#external test as if you are using this library

set(MPEGTS_TESTS_USE_CUSTUM_ASSERT False)

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(NOTICE "[MpegTsTests] using custum assert in Release mode")
    set(MPEGTS_TESTS_USE_CUSTUM_ASSERT True)
endif()

add_library(MpegTsTestsData OBJECT test_data.c)

add_library(MpegTsTestsCommon STATIC)
if (MPEGTS_TESTS_USE_CUSTUM_ASSERT)
    target_sources(MpegTsTestsCommon PRIVATE runtime_assert.c)
endif()

target_link_libraries(MpegTsTestsCommon PUBLIC MpegTS)
target_link_libraries(MpegTsTestsCommon PUBLIC MpegTsTestsData)

add_executable(MpegTsParserTester)
target_sources(MpegTsParserTester PRIVATE parser_test.c)
target_link_libraries(MpegTsParserTester PRIVATE MpegTsTestsCommon)
add_test(MpegTsParserTest MpegTsParserTester)


add_executable(MpegTsPMTBuilderTester)
target_sources(MpegTsPMTBuilderTester PRIVATE pmt_builder_test.c)
target_link_libraries(MpegTsPMTBuilderTester PRIVATE MpegTsTestsCommon)
add_test(MpegTsPMTBuilderTest MpegTsPMTBuilderTester)

