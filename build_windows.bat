@echo off
:: build_windows.bat — Build void_blaster on Windows using MSYS2/MinGW
::
:: SETUP (one time):
::   1. Install MSYS2 from https://www.msys2.org/
::   2. Open "MSYS2 MinGW 64-bit" and run:
::        pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib
::   3. Add MSYS2 to PATH if not already:
::        C:\msys64\mingw64\bin
::   Then just double-click this .bat or run it from cmd.

echo Compiling void_blaster (Windows)...

g++ -std=c++17 -O2 -Wall -Wno-unused-parameter ^
    main.cpp data.cpp game.cpp draw.cpp hud.cpp vfx.cpp world.cpp ^
    -lraylib -lopengl32 -lgdi32 -lwinmm ^
    -o void_blaster.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Build failed. Make sure MSYS2 MinGW64 is installed and on PATH.
    echo See setup instructions at the top of this file.
    pause
    exit /b 1
)

echo Done. Run void_blaster.exe
pause
