clear
mkdir -p ../build/linux
gcc moonbreath.c -O2 -std=c99 -DMOONBREATH_DEBUG=1 -o ../build/linux/moonbreath -lSDL2 -lSDL2_image -lSDL2_ttf
cp -RT ../data ../build/linux/data

echo [Success]

# -Wall -Wextra