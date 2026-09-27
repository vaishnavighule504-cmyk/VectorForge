@echo off
set PATH=C:\msys64\ucrt64\bin;C:\msys64\usr\bin;%PATH%
cmake -B build -G "MinGW Makefiles" > build_out.txt 2>&1
cmake --build build >> build_out.txt 2>&1
