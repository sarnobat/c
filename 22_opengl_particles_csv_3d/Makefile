# Compiler and flags
CC      = cc
CFLAGS  = -Wall -Wextra -O2

# === Choose your toolkit ===
# For classic GLUT code:
USE_GLUT = 1
# For modern GLFW + GLEW code:
# USE_GLUT = 0

# === Library paths (adjust if installed elsewhere) ===
GLEW_PREFIX = /opt/homebrew/Cellar/glew/2.2.0_1
GLFW_PREFIX = /opt/homebrew/Cellar/glfw/3.4

# === Flags ===
ifeq ($(USE_GLUT),1)
    CFLAGS  += -I$(GLEW_PREFIX)/include
    LDFLAGS += -L$(GLEW_PREFIX)/lib -lGLEW -framework GLUT -framework OpenGL
else
    CFLAGS  += -I$(GLEW_PREFIX)/include -I$(GLFW_PREFIX)/include
    LDFLAGS += -L$(GLEW_PREFIX)/lib -lGLEW -L$(GLFW_PREFIX)/lib -lglfw -framework OpenGL
endif

SRC = main.c
BIN = main

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN)
