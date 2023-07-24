cmake_minimum_required(VERSION 3.10)




add_library(MpegTS OBJECT)

target_compile_features(MpegTS PRIVATE "c_std_11")

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

