@echo off
mkdir build 2>nul
del build\tinytcp.exe 2>nul

c++ main.cpp -o build\tinytcp.exe -lws2_32

build\tinytcp.exe %*

