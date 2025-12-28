# Maybe make this more robust in the future.
# Auto detect perhaps?
OPENGL_VERSION = 4.6
C_VERSION = c23
ifeq ($(OPENGL_VERSION), 4.6)
GLAD_PATH = lib/glad_4_6_core
else
GLAD_PATH = lib/glad_3_1_compat
endif

CC      = gcc

# Add these to CFLAGS to check for memory issues -fsanitize=address -fsanitize=undefined
CFLAGS  = -Wall -g -O3 -Iinclude -I$(GLAD_PATH)/include --std=$(C_VERSION) -g
NVIDIA_GL_PATH = /usr/lib/nvidia
LIBS = -L$(NVIDIA_GL_PATH) -lGL -lGLU -lglfw -lm -ldl -lyaml

# break this out into individual object files at some point
SRC     = src/main.c src/physics/gravity.c $(GLAD_PATH)/src/glad.c src/math/math_funcs.c src/utils/constants.c src/utils/utils.c src/math/vector2.c src/physics/cr3bp.c src/math/matrix4.c src/utils/config_parser.c src/utils/shaders_parser.c src/math/vector3.c src/graphics/render3d.c src/physics/gravity3d.c
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
	valgrind --leak-check=full ./$(TARGET) -m n-body -n 4 -t 5000 
