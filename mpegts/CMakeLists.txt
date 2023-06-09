cmake_minimum_required(VERSION 3.10)


add_library(MpegTS OBJECT)


target_sources(MpegTS PRIVATE
	src/parser/MpegTsParser.c
	src/parser/MpegTsParserInitEx.c
	src/parser/MpegTsParserInitStd.c
	)


target_include_directories(MpegTS PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

