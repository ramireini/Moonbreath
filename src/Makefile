CC := gcc

IDIR := inc
LDIR := lib
ODIR := obj
BDIR := build

ADD_LATER_MAYBE := -Werror
COMPILER_FLAGS := -ggdb -Wall -Wextra -O2 -std=c99 -pedantic

SDL := -lSDL2 -lSDL2_image -lSDL2_ttf -lm

_DEPS := dungeon_generation.h game.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ := main.o dungeon_generation.o game.o
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