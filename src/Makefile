CC = gcc

IDIR = inc
ODIR = obj
BDIR = build

COMPILER_FLAGS = -g -Wall -Wextra -O2 -std=c99 -pedantic -I $(IDIR)

LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lm

_DEPS = dungeon_generation.h game.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o dungeon_generation.o game.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: files aaa

$(ODIR)/%.o: %.c
	$(CC) -c $< $(COMPILER_FLAGS) -o $@

aaa: $(OBJ)
	$(CC) $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BDIR)/$@
	@echo "\n[SUCCESS]"

files:
	mkdir -p $(ODIR)
	mkdir -p build
	cp -RTv data build/data/

.PHONY: clean

clean:
	rm -f obj/*.o
	rm -fr build/*
	@echo "\n[SUCCESS]"

full:
	make clean
	clear
	make