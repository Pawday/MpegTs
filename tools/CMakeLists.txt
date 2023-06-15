add_executable(PMTDumper PMTDumper.c)

target_link_libraries(PMTDumper PRIVATE MulticastSocket)
target_link_libraries(PMTDumper PRIVATE MpegTS)
