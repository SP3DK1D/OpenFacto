@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "SCRIPT_DIR=%~dp0"
set "BUILD_TYPE=%~1"
if "%BUILD_TYPE%"=="" set "BUILD_TYPE=Debug"
set "BUILD_DIR=%SCRIPT_DIR%build"

where cmake >nul 2>nul
if errorlevel 1 (
  echo Error: cmake not found in PATH.
  exit /b 1
)

set "GEN="
where ninja >nul 2>nul
if not errorlevel 1 set "GEN=-G Ninja"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%BUILD_DIR%\CMakeCache.txt" (
  cmake -S "%SCRIPT_DIR%" -B "%BUILD_DIR%" %GEN% -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
) else (
  cmake -S "%SCRIPT_DIR%" -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
)
if errorlevel 1 goto :fail

cmake --build "%BUILD_DIR%" --config %BUILD_TYPE%
if errorlevel 1 goto :fail

if exist "%BUILD_DIR%\bin\openfacto.exe" (
  pushd "%SCRIPT_DIR%"
  "%BUILD_DIR%\bin\openfacto.exe"
  popd
  exit /b 0
)
if exist "%BUILD_DIR%\%BUILD_TYPE%\openfacto.exe" (
  pushd "%SCRIPT_DIR%"
  "%BUILD_DIR%\%BUILD_TYPE%\openfacto.exe"
  popd
  exit /b 0
)
if exist "%BUILD_DIR%\openfacto.exe" (
  pushd "%SCRIPT_DIR%"
  "%BUILD_DIR%\openfacto.exe"
  popd
  exit /b 0
)

echo Error: built executable not found.
exit /b 1

:fail
echo Build failed.
exit /b 1
