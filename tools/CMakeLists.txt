add_executable(PMTDumper pmt_dumper.c)

target_link_libraries(PMTDumper PRIVATE MulticastSocket)
target_link_libraries(PMTDumper PRIVATE MpegTS)
