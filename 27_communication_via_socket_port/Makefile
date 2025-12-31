CC ?= cc
CFLAGS ?= -Wall -Wextra -O2
LDFLAGS ?=
PORT ?= 5111

BIN := sender receiver

.PHONY: run
run: receiver
	@echo 'In another tab, run'
	@echo `realpath sender`' 127.0.0.1 $(PORT) "type your message here"'
	./receiver $(PORT)

all: $(BIN)

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

receiver: receiver.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)


.PHONY: clean
clean:
	rm -f $(BIN)
