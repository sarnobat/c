# Compiler and flags
CXX      = c++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17
LDFLAGS  =
BIN      = main
CPPSRC   = main.cpp

# === Choose your toolkit ===
# For classic GLUT code:
USE_GLUT = 0
# For modern GLFW + GLEW code:
# USE_GLUT = 0

# === Homebrew fallback paths (for macOS) ===
BREW_PREFIX = $(shell brew --prefix)
GLEW_INCLUDE = $(BREW_PREFIX)/include
GLEW_LIB     = $(BREW_PREFIX)/lib
GLFW_INCLUDE = $(BREW_PREFIX)/include
GLFW_LIB     = $(BREW_PREFIX)/lib

# === Flags setup ===
ifeq ($(USE_GLUT),1)
    ifneq ($(shell pkg-config --exists glew && echo yes),yes)
        CXXFLAGS += -I$(GLEW_INCLUDE)
        LDFLAGS  += -L$(GLEW_LIB) -lGLEW -framework GLUT -framework OpenGL
    else
        CXXFLAGS += $(shell pkg-config --cflags glew)
        LDFLAGS  += $(shell pkg-config --libs glew) -framework GLUT -framework OpenGL
    endif
else
    ifneq ($(shell pkg-config --exists glew glfw3 && echo yes),yes)
        CXXFLAGS += -I$(GLEW_INCLUDE) -I$(GLFW_INCLUDE)
        LDFLAGS  += -L$(GLEW_LIB) -lGLEW -L$(GLFW_LIB) -lglfw -framework OpenGL
    else
        CXXFLAGS += $(shell pkg-config --cflags glew glfw3)
        LDFLAGS  += $(shell pkg-config --libs glew glfw3) -framework OpenGL
    endif
endif

# === Build target ===
$(BIN): $(CPPSRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# === Run target ===
run: $(BIN)
	./$(BIN) data.csv

# === Clean target ===
clean:
	rm -f $(BIN)
