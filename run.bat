@echo off
mkdir build 2>nul
del build\tinytcp.exe 2>nul

rem Collect all .cpp files into a variable
setlocal enabledelayedexpansion
set sources=
for /r source %%f in (*.cpp) do set sources=!sources! %%f

rem Compile
c++ %sources% -Iheaders -o build\tinytcp.exe -lws2_32

rem Run
build\tinytcp.exe %*

