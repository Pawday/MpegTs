cmake_minimum_required(VERSION 3.10)


list(APPEND GNU_CC_STRICT_MODE_OPTIONS
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pedantic")

list(APPEND MSVC_CC_STRICT_MODE_OPTIONS
	# "/Wall"
	# "/WX"
    )

set(CC_STRICT_MODE_OPTIONS "")
if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC")
    set(CC_STRICT_MODE_OPTIONS ${GNU_CC_STRICT_MODE_OPTIONS})
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang" AND NOT ${CMAKE_C_COMPILER_FRONTEND_VARIANT} STREQUAL "GNU")
    set(CC_STRICT_MODE_OPTIONS ${MSVC_CC_STRICT_MODE_OPTIONS})
else()
    message(NOTICE "[MpegTs]: Unknown compiler, CC_STRICT_MODE_OPTIONS set to empty")
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC")
    # see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
    list(APPEND CC_STRICT_MODE_OPTIONS "-Wno-missing-braces")
endif()

add_library(MpegTS OBJECT)

target_compile_features(MpegTS PRIVATE "c_std_11")

target_compile_options(MpegTS PRIVATE ${CC_STRICT_MODE_OPTIONS}) 


target_sources(MpegTS PRIVATE
    descriptor_converters.c
    descriptor_dumper.c
    descriptor_parser.c
    packet_header_parser.c
    packet_parser.c
    pmt_builder.c
    pmt_dumper.c
    pmt_stream_element.c
    pmt_stream_element_dumper.c
    stream_type.c
)

include(descriptors/descriptor_types.cmake)

target_include_directories(MpegTS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")

