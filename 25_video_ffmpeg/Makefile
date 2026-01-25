APP = slideshow
SRC = slideshow.c
IMG_DIR = /Volumes/new/new/screenshots
IMG_CWD = $(shell pwd)
NUM_IMAGES = 5

STB_URL = https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
STB_FILE = stb_image.h

INCLUDE = -I/opt/homebrew/include
LIBS    = -L/opt/homebrew/lib -lavformat -lavcodec -lavutil -lswscale -lm

.PHONY: run build clean copy_images stb

# Default target: build and run
run: build
	./$(APP)

# Build target depends on downloading stb and copying images
build: stb copy_images $(APP)
	@echo "Slideshow created: slideshow.mp4"

# Download stb_image.h if it doesn't exist
stb:
	@if [ ! -f $(STB_FILE) ]; then \
		echo "Downloading $(STB_FILE)..."; \
		curl -LO $(STB_URL); \
	else \
		echo "$(STB_FILE) already exists"; \
	fi

# Copy 5 random PNGs from source dir
copy_images:
	@echo "Selecting $(NUM_IMAGES) random PNG files from $(IMG_DIR)"
	@rm -f *.png
	@find $(IMG_DIR) -maxdepth 1 -type f -name "*.png" | shuf | head -n $(NUM_IMAGES) | while read f; do \
		cp "$$f" "$(IMG_CWD)/"; \
	done
	@echo "Copied files:"
	@ls *.png

# Compile C program
$(APP): $(SRC) $(STB_FILE)
	cc $(SRC) -o $(APP) $(INCLUDE) $(LIBS)

# Clean up
clean:
	rm -f $(APP) slideshow.mp4 *.png
