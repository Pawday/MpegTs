rm -f ./compile_commands.json
cmake -B BUILD/DEBUG/CLANG_FUZZ \
	-DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	-DCMAKE_C_COMPILER=clang \
	-DCMAKE_C_COMPILER_WORKS=ON \
	-DMPEGTS_ENABLE_TESTING=ON \
	-DMPEGTS_TESTS_FUZZ=ON \

ln -s BUILD/DEBUG/CLANG_FUZZ/compile_commands.json compile_commands.json