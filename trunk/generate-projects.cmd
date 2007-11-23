@echo off

del CMakeCache.txt
cmake -G "Visual Studio 8 2005" -C CMakeMSVCDefaults.txt %* .
