CC      = gcc
CFLAGS  = -Wall -g -O3 -Iinclude
LIBS    = -lm -ldl -lglfw -lGL

SRC     = main.c gravity.c src/glad.c math_funcs.c constants.c
OUTDIR  = build
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
