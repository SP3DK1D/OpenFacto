@echo off
setlocal
cmake -S . -B build
if errorlevel 1 goto :eof
cmake --build build --config Release
if errorlevel 1 goto :eof
if exist build\Release\openfacto.exe (
  start "" build\Release\openfacto.exe
) else (
  start "" build\openfacto.exe
)
