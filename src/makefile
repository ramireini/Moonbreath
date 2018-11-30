CC := gcc

IDIR := inc
LDIR := lib
ODIR := obj
BDIR := build

# -O0, faster compilation, slower execution, not optimized
# -O2, slower compilation, faster execution, optimized

ADD_LATER := -Werror -O2 -fdiagnostics-color=always
COMPILER_FLAGS := -ggdb -Wall -Wextra -O0 -std=c99 -pedantic -D DEBUG=0 -I $(IDIR)
LIBS := -lSDL2 -lSDL2_image -lSDL2_ttf -lm

_DEPS := game.h level_generation.h item.h render.h text_render.h update.h util_io.h util_conf.h util_math.h tiles.h types.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ := main.o game.o level_generation.o item.o render.o text_render.o update.o util_io.o
OBJ := $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: clean
make: clean files aaa

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
	$(CC) $^ $(COMPILER_FLAGS) -o $(BDIR)/$@ $(LIBS)
	@echo "[COMPILATION SUCCESS]\n"

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(COMPILER_FLAGS) -c $< -o $@