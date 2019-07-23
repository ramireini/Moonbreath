#!/bin/bash

clear

if [ "$1" == "linux" ]; then
  mkdir -p ../build/linux
  gcc moonbreath.c  -O2 -std=c99 -DMOONBREATH_DEBUG=1 -o ../build/linux/Moonbreath -lSDL2 -lSDL2_image -lSDL2_ttf
  cp -RT ../data ../build/linux/data

  echo "[Linux Success]"
elif [ "$1" == "windows" ]; then
  mkdir -p ../build/windows
  x86_64-w64-mingw32-gcc moonbreath.c -I.. -O2 -std=c99 -DMOONBREATH_DEBUG=1 -L../lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf ../res/icon.o -mwindows -o ../build/windows/Moonbreath.exe
  cp -RT ../data ../build/windows/data
  cp -RT ../dll ../build/windows

  echo "[Windows Success]"
else
  echo "Not a valid target system"
fi