add_executable(StreamDumper StreamDumper.c)
add_executable(PIDDumper PidDumper.c)

target_link_libraries(StreamDumper PRIVATE MulticastSocket)

target_link_libraries(PIDDumper PRIVATE MulticastSocket)
target_link_libraries(PIDDumper PRIVATE MpegTS)
