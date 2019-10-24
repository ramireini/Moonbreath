#!/bin/bash

clear

if [ "$1" == "linux" ]; then
  mkdir -p ../build/linux
  time gcc moonbreath.c -I ../ -O2 -std=c99 -DMOONBREATH_SLOW=1 -o ../build/linux/Moonbreath -lSDL2 -lSDL2_image -lSDL2_ttf
  cp -RT ../data ../build/linux/data

  echo "[Linux Success]"
elif [ "$1" == "windows" ]; then
  mkdir -p ../build/windows
  time x86_64-w64-mingw32-gcc moonbreath.c -I ../ -O2 -std=c99 -DMOONBREATH_SLOW=1 -o ../build/windows/Moonbreath.exe -L ../SDL2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf ../res/icon.o -mwindows
  cp -RT ../data ../build/windows/data
  cp -RT ../SDL2/dll ../build/windows

  echo "[Windows Success]"
else
  echo "Usage: ./build.sh linux/windows"
fi

# TODO(rami): Going to need to turn on -Wall and -Wextra later and clean up whatever needs to be cleaned up

# TODO(rami): Remove time calls