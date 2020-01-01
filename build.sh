#!/bin/bash
ctags -R .
mkdir -p build && cd build
CXX=/opt/local/bin/clang++-mp-9.0 cmake .. #-DCMAKE_EXPORT_COMPILE_COMMANDS=1
make -j4
