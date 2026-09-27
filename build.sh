#!/bin/bash
export PATH=/ucrt64/bin:/usr/bin:$PATH
cd /c/Users/vaish/OneDrive/Desktop/VectorForge
/ucrt64/bin/cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=/ucrt64 > cmake.log 2>&1
/ucrt64/bin/mingw32-make -C build -j4 > make.log 2>&1
