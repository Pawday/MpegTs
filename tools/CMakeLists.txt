add_executable(PMTDumper pmt_dumper_launcher.c)

target_link_libraries(PMTDumper PRIVATE MulticastSocket)
target_link_libraries(PMTDumper PRIVATE MpegTS)
