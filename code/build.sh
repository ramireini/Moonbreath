clear
mkdir -p ../build
cp -RTv ../data ../build/data/

gcc linux_moonbreath.c -Wall -Wextra -O2 -std=c99 -DMOONBREATH_DEBUG=1 -o ../build/moonbreathgayboi -lSDL2 -lSDL2_image -lSDL2_ttf -lm

echo [COMPLETE]
