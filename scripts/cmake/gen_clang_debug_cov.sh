rm -f ./compile_commands.json
cmake -B BUILD/DEBUG/CLANG_COV \
	-DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	-DCMAKE_C_COMPILER=clang \
	-DMPEGTS_ENABLE_TESTING=ON \
	-DCMAKE_C_FLAGS_INIT="-fprofile-instr-generate -fcoverage-mapping"

ln -s BUILD/DEBUG/CLANG_COV/compile_commands.json compile_commands.json

run_tests_file="./BUILD/DEBUG/CLANG_COV/build_and_ctest_with_cov.sh"
rm -f ${run_tests_file}

echo "rm -rf ./llvm_covs"                                                               >> ${run_tests_file}
echo "cmake --build ."                                                                  >> ${run_tests_file}
echo "LLVM_PROFILE_FILE=\"../llvm_covs/mpeg_ctest_%m.profraw\" ctest"                   >> ${run_tests_file}
echo "llvm-profdata merge -sparse ./llvm_covs/*.profraw -o ./llvm_covs/Merged.profdata" >> ${run_tests_file}
echo "llvm-cov show --format=html tests/libMpegTsTestsCommon.a  -instr-profile=llvm_covs/Merged.profdata > COV_REPORT.html" >> ${run_tests_file}

