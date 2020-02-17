#!/bin/bash
ctags -R .
mkdir -p build && cd build
#CXX=/opt/local/bin/clang++-mp-9.0 cmake .. #-DCMAKE_EXPORT_COMPILE_COMMANDS=1
#CXX=/opt/local/bin/g++-mp-9 CC=/opt/local/bin/gcc-mp-9
CXX=/opt/local/bin/clang++-mp-9.0 CC=/opt/local/bin/clang-mp-9.0
cmake ..
cmake --build .
