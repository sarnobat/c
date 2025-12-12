CC      ?= cc
CFLAGS  ?= -O2 -Wall -Wextra -std=c11
TARGET  := git-uncommitted
SRC     := git-uncommitted.c

.PHONY: all clean

all: $(TARGET) test

test:
	ls -d /Volumes/git/github/*  | ./git-uncommitted --long

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
