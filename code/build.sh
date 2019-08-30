#!/bin/bash

clear

if [ "$1" == "linux" ]; then
  mkdir -p ../build/linux
  gcc moonbreath.c -O2 -std=c99 -o ../build/linux/Moonbreath -lSDL2 -lSDL2_image -lSDL2_ttf -lm
  cp -RT ../data ../build/linux/data

  echo "[Linux Success]"
elif [ "$1" == "windows" ]; then
  mkdir -p ../build/windows
  x86_64-w64-mingw32-gcc moonbreath.c -I.. -O2 -std=c99 -o ../build/windows/Moonbreath.exe -L ../lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf ../res/icon.o -mwindows -lm
  cp -RT ../data ../build/windows/data
  cp -RT ../dll ../build/windows

  echo "[Windows Success]"
else
  echo "Usage ./build.sh [Target]"
fi

# TODO(rami): Going to need to turn on -Wall and -Wextra later and clean up whatever needs to be cleaned up