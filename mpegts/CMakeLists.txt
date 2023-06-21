cmake_minimum_required(VERSION 3.10)


add_library(MpegTS OBJECT)


target_sources(MpegTS PRIVATE
	src/packet_header_parser.c
	src/packet_inplace_parser.c
	src/descriptor_converters.c
	src/descriptor_dumper.c
	src/descriptor_parser.c
	src/pmt_builder.c
	src/pmt_dumper.c
	src/pmt_stream_element.c
	src/pmt_stream_element_dumper.c
	src/stream_type.c
	)


target_include_directories(MpegTS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

