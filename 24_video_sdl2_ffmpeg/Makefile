APP = sdl_hello
SRC = main.c

SDL2_INC = /opt/homebrew/include
SDL2_LIB = /opt/homebrew/lib

CFLAGS = -I$(SDL2_INC)
LDFLAGS = -L$(SDL2_LIB) -lSDL2

.PHONY: run build install clean

run: build
	./$(APP)

build: install $(APP)

$(APP): $(SRC)
	cc $(CFLAGS) $(SRC) -o $(APP) $(LDFLAGS)

install:
	@if [ ! -d "/opt/homebrew/Cellar/sdl2" ]; then \
		echo "Installing SDL2..."; \
		brew install sdl2; \
	fi

clean:
	rm -f $(APP)
