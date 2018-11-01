CC := gcc

IDIR := inc
LDIR := lib
ODIR := obj
BDIR := build

# -O0, faster compilation, slower execution, not optimized
# -O2, slower compilation, faster execution, optimized

ADD_LATER_MAYBE := -Werror -O2
COMPILER_FLAGS := -ggdb -Wall -Wextra -O0 -std=c99 -pedantic -D DEBUG=1

SDL := -lSDL2 -lSDL2_image -lSDL2_ttf -lm

_DEPS := dungeon_generation.h moonbreath_mountain.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ := game.o dungeon_generation.o moonbreath_mountain.o
OBJ := $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: clean
all: files aaa

aaa: $(OBJ)
	$(CC) $^ $(COMPILER_FLAGS) -o $(BDIR)/$@ $(SDL)
	@echo "\n[SUCCESS]"

$(ODIR)/%.o: %.c
	$(CC) $(COMPILER_FLAGS) -c $< -o $@ -I $(IDIR)

clean:
	rm -fr obj
	rm -fr build
	@echo "\n[SUCCESS]"

files:
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)
	cp -RTv data build/data/

full:
	make clean
	clear
	make