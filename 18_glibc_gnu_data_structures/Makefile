CC = gcc
CFLAGS = -Wall -g
GLIB_FLAGS = `pkg-config --cflags --libs glib-2.0`

# Source files
GNU_SOURCES = hashgnu.c listgnu.c treegnu.c
POSIX_SOURCES = hashposix.c listposix.c treeposix.c

# Executables (strip .c extension)
GNU_TARGETS = $(GNU_SOURCES:.c=)
POSIX_TARGETS = $(POSIX_SOURCES:.c=)
ALL_TARGETS = $(GNU_TARGETS) $(POSIX_TARGETS)

# Without .PHONY, if you created actual files named all, build, run, clean, or list in your directory, make would think those targets are up-to-date and skip them.
# A phony target is one that is not really the name of a file; rather it is just a name for a recipe to be executed when you make an explicit request. There are two reasons to use a phony target: to avoid a conflict with a file of the same name, and to improve performance.
# Because the rm command does not create a file named clean, probably no such file will ever exist. Therefore, the rm command will be executed every time you say ‘make clean’.
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