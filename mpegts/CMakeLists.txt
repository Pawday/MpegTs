cmake_minimum_required(VERSION 3.10)


add_library(MpegTS OBJECT)


target_sources(MpegTS PRIVATE
	src/packet/parser/init_extended.c
	src/packet/parser/init_with_stdlib.c
	src/packet/parser/data_ops.c
	src/packet/parser/parse_ops.c
	src/psi/stream_type.c
	src/psi/descriptor/converters.c
	src/psi/descriptor/parser.c
	)


target_include_directories(MpegTS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(tests)
