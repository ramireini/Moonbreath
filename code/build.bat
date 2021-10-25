@echo off
cls

:: Recreate build folder
rd /s /q "../build/windows"
mkdir "../build/windows"

:: Build icon
pushd ..\res
windres icon.rc -O coff -o icon.res
popd

gcc moonbreath.c -I ../ -O0 -std=c99 -DMOONBREATH_WINDOWS=1 -DMOONBREATH_SLOW=1 -o ../build/windows/Moonbreath.exe -L ../SDL2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lm ../res/icon.res -mwindows

:: Copy data and DLL's into the build folder
robocopy ../data ../build/windows/data /E /NFL /NDL /NJH /NJS /nc /ns /np
robocopy ../SDL2/dll ../build/windows /E /NFL /NDL /NJH /NJS /nc /ns /np