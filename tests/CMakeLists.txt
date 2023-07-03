#external test as if you are using this library

enable_testing()
add_library(MpegTsTestsData OBJECT test_data.c)

add_library(MpegTsTestsCommon STATIC)

target_link_libraries(MpegTsTestsCommon PUBLIC MpegTS)
target_link_libraries(MpegTsTestsCommon PUBLIC MpegTsTestsData)

add_executable(MpegTsParserTester parser_test.c)
target_link_libraries(MpegTsParserTester PRIVATE MpegTsTestsCommon)
add_test(MpegTsParserTest MpegTsParserTester)


add_executable(MpegTsPMTBuilderTester pmt_builder_test.c)
target_link_libraries(MpegTsPMTBuilderTester PRIVATE MpegTsTestsCommon)
add_test(MpegTsPMTBuilderTest MpegTsPMTBuilderTester)
