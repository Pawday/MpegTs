cmake_minimum_required(VERSION 3.10)


add_library(MpegTS OBJECT)


target_sources(MpegTS PRIVATE
	src/packet/parser/alloc_free.c
	src/packet/parser/clone_packet.c
	src/packet/parser/data_ops.c
	src/packet/parser/init_extended.c
	src/packet/parser/inplace_parser.c
	src/packet/parser/packet_header.c
	src/packet/parser/parse_ops.c
	src/psi/descriptor/converters.c
	src/psi/descriptor/dumper.c
	src/psi/descriptor/parser.c
	src/psi/pmt_builder.c
	src/psi/pmt_dumper.c
	src/psi/pmt_single_packet_parser.c
	src/psi/pmt_stream_element.c
	src/psi/pmt_stream_element_dumper.c
	src/psi/stream_type.c
	)


target_include_directories(MpegTS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

