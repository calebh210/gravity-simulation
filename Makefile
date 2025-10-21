CC = gcc
CFLAGS = -Wall -g -O3
LIBS = -lm -ldl -lglfw -lGL -Iinclude

all: clean sim

clean: 
	rm -f build/*

sim: gravity.c
	$(CC) $(CFLAGS) gravity.c src/glad.c $(CFLAGS) $(LIBS) -o build/sim

check: 
	valgrind --leak-check=full ./build/sim
