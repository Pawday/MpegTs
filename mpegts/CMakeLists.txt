cmake_minimum_required(VERSION 3.10)


add_library(MpegTS OBJECT)


target_sources(MpegTS PRIVATE
	src/packet/packet_header_parser.c
	src/packet/packet_inplace_parser.c
	src/psi/descriptor_converters.c
	src/psi/descriptor_dumper.c
	src/psi/descriptor_parser.c
	src/psi/pmt_builder.c
	src/psi/pmt_dumper.c
	src/psi/pmt_stream_element.c
	src/psi/pmt_stream_element_dumper.c
	src/psi/stream_type.c
	)


target_include_directories(MpegTS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

