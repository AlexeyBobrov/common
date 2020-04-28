#!/bin/bash

mkdir -p build && cd build
cmake .. #-DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build . --target test
cmake --build . --target gcov
