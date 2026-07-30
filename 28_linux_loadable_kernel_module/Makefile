obj-m := hello_lkm.o

.PHONY: all build check-linux setup install uninstall reload test clean

all: build
	@echo "next thing to learn: bpftrace one-liners using kernel tracepoints (instead of kernel module)"

build: hello_lkm.ko

hello_lkm.ko: hello_lkm.c
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

check-linux:
	@test "`uname -s`" != "Linux" && { echo "This module can only run on Linux, not `uname -s`."; exit 1; } || true

setup: check-linux
	@test ! -x "`command -v apt-get 2>/dev/null`" && { echo "setup requires apt-get. Install build-essential, linux-headers-`uname -r`, and gcc-12 manually."; exit 1; } || true
	sudo apt-get update
	sudo apt-get install -y build-essential linux-headers-`uname -r` gcc-12

install: check-linux hello_lkm.ko
	@lsmod | grep -q '^hello_lkm\b' && { echo "hello_lkm is already loaded; unloading it first."; sudo rmmod hello_lkm; } || true
	sudo insmod hello_lkm.ko
	@echo "Loaded hello_lkm. Check dmesg with: sudo dmesg | tail"

uninstall: check-linux
	sudo rmmod hello_lkm

reload: install

test: install
	sudo dmesg | grep 'SRIDHAR .*hello_lkm' | tail -20

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
