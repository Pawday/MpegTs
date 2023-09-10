cmake_minimum_required(VERSION 3.10)


target_compile_options(MpegTS PRIVATE "-fsanitize=fuzzer-no-link")


add_executable(Fuzz MpegTSPMTBuilderFuzz.c)
target_compile_options(Fuzz PRIVATE "-fsanitize=fuzzer")
target_link_options(Fuzz PRIVATE "-fsanitize=fuzzer")
target_link_libraries(Fuzz PRIVATE MpegTS)
