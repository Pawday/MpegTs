#external test as if you are using this library

list(APPEND GNU_CC_STRICT_MODE_OPTIONS
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pedantic")

list(APPEND MSVC_CC_STRICT_MODE_OPTIONS
    "/Wall"
)

set(CC_STRICT_MODE_OPTIONS "")
if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC")
    set(CC_STRICT_MODE_OPTIONS ${GNU_CC_STRICT_MODE_OPTIONS})
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang" AND NOT ${CMAKE_C_COMPILER_FRONTEND_VARIANT} STREQUAL "GNU")
    set(CC_STRICT_MODE_OPTIONS ${MSVC_CC_STRICT_MODE_OPTIONS})
else()
    message(NOTICE "[MpegTsTests]: Unknown compiler, CC_STRICT_MODE_OPTIONS set to empty")
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC")
    # see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
    list(APPEND CC_STRICT_MODE_OPTIONS "-Wno-missing-braces")
endif()

add_library(MpegTsTestsCommon STATIC)

if (NOT MSVC AND ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    target_compile_options(MpegTsTestsCommon PUBLIC "-fsanitize=address")
    target_link_options(MpegTsTestsCommon PUBLIC "-fsanitize=address")
endif()

target_compile_options(MpegTsTestsCommon PUBLIC ${CC_STRICT_MODE_OPTIONS})

add_library(MpegTsTestsData OBJECT test_data.c)

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

