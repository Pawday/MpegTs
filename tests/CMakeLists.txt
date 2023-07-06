#external test as if you are using this library


add_library(MpegTsTestsData OBJECT test_data.c)

add_library(MpegTsTestsCommon STATIC)


if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    target_compile_options(MpegTsTestsCommon PUBLIC "-Wall" "-Wextra" "-Werror" "-pedantic")
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC")
    target_compile_options(MpegTsTestsCommon PUBLIC "-Wno-missing-braces") # see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
endif()

target_link_libraries(MpegTsTestsCommon PUBLIC MpegTS)
target_link_libraries(MpegTsTestsCommon PUBLIC MpegTsTestsData)

add_executable(MpegTsParserTester parser_test.c)
target_link_libraries(MpegTsParserTester PRIVATE MpegTsTestsCommon)
add_test(MpegTsParserTest MpegTsParserTester)


add_executable(MpegTsPMTBuilderTester pmt_builder_test.c)
target_link_libraries(MpegTsPMTBuilderTester PRIVATE MpegTsTestsCommon)
add_test(MpegTsPMTBuilderTest MpegTsPMTBuilderTester)
