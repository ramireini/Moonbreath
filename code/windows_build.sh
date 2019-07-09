clear
mkdir -p ../build/windows
x86_64-w64-mingw32-gcc -I.. -O2 moonbreath.c -o ../build/windows/moonbreath.exe -L../lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows
cp -RT ../data ../build/windows/data
cp -RT ../dll ../build/windows

echo [Success]

# -Wall -Wextra