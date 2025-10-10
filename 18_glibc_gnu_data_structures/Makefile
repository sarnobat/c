CC = gcc
CFLAGS = -Wall -g
GLIB_FLAGS = `pkg-config --cflags --libs glib-2.0`

# Source files
GNU_SOURCES = hashgnu.c listgnu.c
POSIX_SOURCES = hashposix.c listposix.c treeposix.c

# Executables (strip .c extension)
GNU_TARGETS = $(GNU_SOURCES:.c=)
POSIX_TARGETS = $(POSIX_SOURCES:.c=)
ALL_TARGETS = $(GNU_TARGETS) $(POSIX_TARGETS)

.PHONY: all build run clean

all: build

build: $(ALL_TARGETS)

# Pattern rule for GNU programs (with glib)
$(GNU_TARGETS): %: %.c
	$(CC) $(CFLAGS) -o $@ $< $(GLIB_FLAGS)

# Pattern rule for POSIX programs (no external libs)
$(POSIX_TARGETS): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

run: build
	@for prog in $(ALL_TARGETS); do \
		if [ -f $$prog ]; then \
			echo "Running $$prog..."; \
			./$$prog; \
			echo ""; \
		fi \
	done

clean:
	rm -f $(ALL_TARGETS) a.out *.o

# Show what would be built
list:
	@echo "GNU targets: $(GNU_TARGETS)"
	@echo "POSIX targets: $(POSIX_TARGETS)"