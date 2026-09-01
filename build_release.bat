@echo off
setlocal

rem =====================================================================
rem Build pfm.exe + PolyFileManager.exe and package a portable win64 ZIP release.
rem
rem   QTDIR    : your Qt kit       (default C:\Qt\6.11.2\mingw_64)
rem   QT_MINGW : MinGW matching Qt (default C:\Qt\Tools\mingw1310_64\bin)
rem   VERSION  : version used in the zip name (default 0.1.0)
rem
rem The GUI's Qt runtime is deployed by windeployqt during the build; this
rem script gathers executables + runtime into dist/ and zips them with tar.
rem =====================================================================

if "%QTDIR%"=="" set "QTDIR=C:\Qt\6.11.2\mingw_64"
if "%QT_MINGW%"=="" set "QT_MINGW=C:\Qt\Tools\mingw1310_64\bin"
if "%VERSION%"=="" set "VERSION=0.1.0"

if not exist "%QT_MINGW%\g++.exe" (
    echo [ERROR] MinGW not found at "%QT_MINGW%". Set QT_MINGW.
    exit /b 1
)
if not exist "%QTDIR%\bin\qmake.exe" (
    echo [ERROR] Qt not found at "%QTDIR%". Set QTDIR.
    exit /b 1
)

set "PATH=%QT_MINGW%;%QTDIR%\bin;%PATH%"

echo === Building (release_build) ===
cmake -S . -B release_build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QTDIR%
if errorlevel 1 exit /b 1
cmake --build release_build --target pfm
if errorlevel 1 exit /b 1
cmake --build release_build --target PolyFileManager
if errorlevel 1 exit /b 1

echo === Staging into dist ===
set "STAGE=dist"
if exist "%STAGE%" rmdir /s /q "%STAGE%"
mkdir "%STAGE%"

copy /y "release_build\gui\PolyFileManager.exe" "%STAGE%\" >nul
copy /y "release_build\gui\*.dll" "%STAGE%\" >nul
for /d %%d in (release_build\gui\*) do (
    if not "%%~nxd"=="CMakeFiles" if not "%%~nxd"=="PolyFileManager_autogen" (
        xcopy /e /i /y "%%d" "%STAGE%\%%~nxd\" >nul
    )
)

copy /y "release_build\pfm.exe" "%STAGE%\" >nul
for %%d in (libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll) do (
    if exist "%QT_MINGW%\%%d" copy /y "%QT_MINGW%\%%d" "%STAGE%\" >nul
)

> "%STAGE%\README.txt" echo Run PolyFileManager.exe to open the GUI; use pfm.exe from the command line.
>> "%STAGE%\README.txt" echo Portable build: extract and run directly, no installation needed.

echo === Zipping ===
pushd "%STAGE%"
tar -a -c -f "..\PolyFileManager-%VERSION%-win64.zip" .
popd
if errorlevel 1 exit /b 1

echo.
echo Done: PolyFileManager-%VERSION%-win64.zip
endlocal
