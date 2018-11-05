CC := gcc

IDIR := inc
LDIR := lib
ODIR := obj
BDIR := build

# -O0, faster compilation, slower execution, not optimized
# -O2, slower compilation, faster execution, optimized

ADD_LATER_MAYBE := -Werror -O2
COMPILER_FLAGS := -ggdb -Wall -Wextra -O0 -std=c99 -pedantic -D DEBUG=0 -fdiagnostics-color=always

SDL := -lSDL2 -lSDL2_image -lSDL2_ttf -lm

_DEPS := moonbreath_mountain.h level_generation.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ := game.o moonbreath_mountain.o level_generation.o util_conf.o
OBJ := $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: clean
all: clean files aaa

clean:
	rm -fr $(ODIR)
	rm -fr $(BDIR)
	@echo "[CLEAN]\n"

files:
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)
	cp -RTv data build/data/
	@echo "[FILES]\n"

aaa: $(OBJ)
	$(CC) $^ $(COMPILER_FLAGS) -o $(BDIR)/$@ $(SDL)
	@echo "[COMPILATION SUCCESS]\n"

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(COMPILER_FLAGS) -c $< -o $@ -I $(IDIR)