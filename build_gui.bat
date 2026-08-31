@echo off
setlocal

rem ---------------------------------------------------------------------------
rem Build the Qt GUI with the MinGW toolchain that MATCHES your Qt kit.
rem
rem   1) Set QTDIR to your Qt kit (the one that contains bin\qmake.exe).
rem         set QTDIR=C:\Qt\6.11.2\mingw_64
rem   2) Make sure the MinGW that Qt was built with is on PATH, or set QT_MINGW.
rem      (It is usually under C:\Qt\Tools\mingwXXXX_64\bin.)
rem
rem Using a different MinGW than Qt's causes a runtime "cannot locate entry
rem point" error because the deployed libstdc++ does not match the exe.
rem A fresh build directory (build_gui) avoids a stale cached compiler.
rem ---------------------------------------------------------------------------

if "%QTDIR%"=="" set "QTDIR=C:\Qt\6.11.2\mingw_64"
if "%QT_MINGW%"=="" set "QT_MINGW=C:\Qt\Tools\mingw1310_64\bin"

if not exist "%QT_MINGW%\g++.exe" (
    echo [ERROR] MinGW not found at "%QT_MINGW%".
    echo         Set QT_MINGW to your Qt's MinGW bin directory, or put it on PATH.
    exit /b 1
)
if not exist "%QTDIR%\bin\qmake.exe" (
    echo [ERROR] Qt not found at "%QTDIR%".
    echo         Set QTDIR to your Qt kit, e.g. set QTDIR=C:\Qt\6.10\mingw_64.
    exit /b 1
)

set "PATH=%QT_MINGW%;%QTDIR%\bin;%PATH%"
echo Compiler: & "%QT_MINGW%\g++.exe" --version 2>nul | findstr /i "g++"
echo.

cmake -S . -B build_gui -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QTDIR%
if errorlevel 1 exit /b 1

cmake --build build_gui --target qt_gui
if errorlevel 1 exit /b 1

echo.
echo Done: build_gui\gui\qt_gui.exe  (Qt runtime already deployed beside it)
endlocal
