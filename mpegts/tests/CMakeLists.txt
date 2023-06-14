add_executable(ParseSyncedTest parse_synced_test.c test_data.c)

target_link_libraries(ParseSyncedTest PRIVATE MpegTS)
