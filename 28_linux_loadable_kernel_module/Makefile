obj-m := hello_lkm.o

KERNEL_RELEASE := $(shell uname -r)
KERNEL_BUILD := /lib/modules/$(KERNEL_RELEASE)/build
PWD := $(shell pwd)

.PHONY: all build check-linux setup install uninstall clean

all: build

build:
	$(MAKE) -C $(KERNEL_BUILD) M=$(PWD) modules

check-linux:
	@if [ "$$(uname -s)" != "Linux" ]; then \
		echo "This module can only run on Linux, not $$(uname -s)."; \
		exit 1; \
	fi

setup: check-linux
	@if ! command -v apt-get >/dev/null 2>&1; then \
		echo "setup requires apt-get. Install build-essential, linux-headers-$(KERNEL_RELEASE), and gcc-12 manually."; \
		exit 1; \
	fi
	sudo apt-get update
	sudo apt-get install -y build-essential linux-headers-$(KERNEL_RELEASE) gcc-12

install: check-linux build
	sudo insmod hello_lkm.ko
	@echo "Loaded hello_lkm. Check dmesg with: sudo dmesg | tail"

uninstall: check-linux
	sudo rmmod hello_lkm

clean:
	$(MAKE) -C $(KERNEL_BUILD) M=$(PWD) clean
