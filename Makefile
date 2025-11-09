CC      = gcc
CFLAGS  = -Wall -g -O3 -Iinclude
LIBS    = -lm -ldl -lglfw -lGL -lyaml

# break this out into individual object files at some point
SRC     = src/main.c src/physics/gravity.c src/glad.c src/math/math_funcs.c src/utils/constants.c src/utils/utils.c src/math/vector2.c src/physics/cr3bp.c
OUTDIR  = out
TARGET  = $(OUTDIR)/sim

all: $(TARGET)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(SRC) | $(OUTDIR)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

clean:
	rm -f $(OUTDIR)/*

check: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)
