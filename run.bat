@echo off
REM Create build folder if it doesn't exist
mkdir build 2>nul

REM Remove old binary
del build\tinytcp.exe 2>nul

REM Collect all .cpp files in source/
setlocal enabledelayedexpansion
set sources=
for /r source %%f in (*.cpp) do set sources=!sources! %%f

REM Compile all .cpp files, include headers, static link
c++ -Iheaders !sources! -std=c++17 -O2 -g -static -static-libgcc -static-libstdc++ -o build\tinytcp.exe -lws2_32

REM Run the executable with any arguments passed
build\tinytcp.exe %*
