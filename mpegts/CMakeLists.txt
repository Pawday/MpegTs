cmake_minimum_required(VERSION 3.10)


add_library(MpegTS OBJECT)


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


target_include_directories(MpegTS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../")

