cmake_minimum_required(VERSION 3.10)


add_library(MpegTS OBJECT)


if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    target_compile_options(MpegTS PRIVATE "-Wall" "-Wextra" "-Werror" "-pedantic")
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL "GCC")
    target_compile_options(MpegTS PRIVATE "-Wno-missing-braces") # see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
endif()


target_sources(MpegTS PRIVATE
	packet_header_parser.c
	packet_inplace_parser.c
	descriptor_converters.c
	descriptor_dumper.c
	descriptor_parser.c
	pmt_builder.c
	pmt_dumper.c
	pmt_stream_element.c
	pmt_stream_element_dumper.c
	stream_type.c
	)

include(descriptors/descriptor_types.cmake)

target_include_directories(MpegTS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")

