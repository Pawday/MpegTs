del compile_commands.json
cmake -B BUILD/DEBUG/CLANG -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_C_COMPILER=clang -G Ninja
mklink /h compile_commands.json BUILD\DEBUG\CLANG\compile_commands.json 
