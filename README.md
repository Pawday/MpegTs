# MPEG Transport Stream Program map table dumper


## Build steps


1. Configure with host's C compiler and CMake
```sh
sh ./scripts/cmake/gen_hostcc_release.sh
```

Optional cppcheck the project
```sh
cppcheck --enable=all --project=compile_commands.json
```

2. Build
```sh
cmake --build ./BUILD/RELEASE/HOSTCC
```

## Usage
```sh
./BUILD/RELEASE/HOSTCC/tools/PMTDumper
```
