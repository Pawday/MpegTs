cmake_minimum_required(VERSION 3.10)


add_library(MpegTS OBJECT)


target_sources(MpegTS PRIVATE
	src/parser/init_extended.c
	src/parser/init_with_stdlib.c
	src/parser/data_ops.c
	src/parser/parse_ops.c
	src/psi/parser.c
	)


target_include_directories(MpegTS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(tests)
