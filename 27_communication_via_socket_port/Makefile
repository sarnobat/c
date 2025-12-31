CC ?= cc
CFLAGS ?= -Wall -Wextra -O2
LDFLAGS ?=
PORT ?= 5000

BIN := sender receiver

all: $(BIN)

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

receiver: receiver.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: run
run: receiver
	@echo 'In another tab, run "./sender 127.0.0.1 $(PORT) \"type your message here\""'
	./receiver $(PORT)

.PHONY: clean
clean:
	rm -f $(BIN)
