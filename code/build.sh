clear
mkdir -p ../build
cp -RT ../data ../build/data/

# gcc linux_moonbreath.c -Wall -Wextra -O2 -std=c99 -DMOONBREATH_DEBUG=1 -o ../build/moonbreath -lSDL2 -lSDL2_image -lSDL2_ttf -lm

gcc linux_moonbreath.c -O2 -std=c99 -DMOONBREATH_DEBUG=1 -o ../build/moonbreath -lSDL2 -lSDL2_image -lSDL2_ttf -lm

echo [COMPLETE]