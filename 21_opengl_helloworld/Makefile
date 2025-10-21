

CC      = cc
CFLAGS  = -Wall -Wextra -O2 $(shell pkg-config --cflags glfw3 glew)
LDFLAGS = $(shell pkg-config --libs glfw3 glew)

SRC     = main.c
BIN     = main

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN)	
	
build:
	rsync -a -v /Volumes/git/src.git/2021/c/2019_opengl/main.c .
	gcc main.c 
	
init:
	brew install glfw glew
	