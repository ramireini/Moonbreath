clear
mkdir -p ../build
gcc linux_moonbreath.c -O2 -std=c99 -DMOONBREATH_DEBUG=1 -o ../build/moonbreath -lSDL2 -lSDL2_image -lSDL2_ttf -lm
cp -RT ../data ../build/data/

# -Wall -Wextra

echo [Success]