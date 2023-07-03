target_sources(MpegTS PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/language_descriptor_parser.c
	${CMAKE_CURRENT_LIST_DIR}/language_descriptor_dumper.c
	${CMAKE_CURRENT_LIST_DIR}/language_descriptor_converters.c
	)

target_sources(MpegTS PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/teletext_descriptor_parser.c
	${CMAKE_CURRENT_LIST_DIR}/teletext_descriptor_converters.c
	${CMAKE_CURRENT_LIST_DIR}/teletext_descriptor_dumper.c
	)
