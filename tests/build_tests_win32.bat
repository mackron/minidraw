@echo off
SET c_compiler=clang
SET cpp_compiler=clang++
SET options=-Wall -Wpedantic -std=c89 -ansi -pedantic -lgdi32 -luser32
@echo on

%c_compiler% minidraw_test_0_automated.c -o ./bin/minidraw_test_0_automated.exe %options%
%c_compiler% minidraw_test_1_graphics.c  -o ./bin/minidraw_test_0_graphics.exe  %options%