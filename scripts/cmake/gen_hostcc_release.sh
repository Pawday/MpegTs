rm -f ./compile_commands.json
cmake -B BUILD/RELEASE/HOSTCC -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
ln -s BUILD/RELEASE/HOSTCC/compile_commands.json compile_commands.json
