CC := gcc

IDIR := inc
LDIR := lib
ODIR := obj
BDIR := build

# -O0, faster compilation, slower execution, not optimized
# -O2, slower compilation, faster execution, optimized

ADD_LATER := -O2 -Werror -fdiagnostics-color=always
COMPILER_FLAGS := -O0 -Wall -Wextra -Wpedantic -std=c99 -D DEBUG=0 -I $(IDIR)
LIBS := -lSDL2 -lSDL2_image -lSDL2_ttf -lm

_DEPS := game.h level_gen.h item.h render.h text_render.h update.h player.h slime.h util_io.h interface.h util_conf.h types.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ := main.o game.o level_gen.o item.o render.o text_render.o update.o player.o slime.o util_io.o interface.o
OBJ := $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: clean
make: files aaa

files:
	rm -rf $(ODIR)
	rm -rf $(BDIR)
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)
	cp -RTv data build/data/
	@echo "[FILES]\n"

aaa: $(OBJ)
	$(CC) $^ $(COMPILER_FLAGS) -o $(BDIR)/$@ $(LIBS)
	@echo "[COMPILATION SUCCESS]\n"

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(COMPILER_FLAGS) -c $< -o $@