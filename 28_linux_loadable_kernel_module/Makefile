obj-m := hello_lkm.o
obj-m += count_user_processes_lkm.o

.PHONY: all build check-linux setup install uninstall reload test clean

all: build
	@echo "now run make install"

build: hello_lkm.ko count_user_processes_lkm.ko

hello_lkm.ko: hello_lkm.c Makefile
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

count_user_processes_lkm.ko: count_user_processes_lkm.c Makefile
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

check-linux:
	@test "`uname -s`" != "Linux" && { echo "This module can only run on Linux, not `uname -s`."; exit 1; } || true

setup: check-linux
	@test ! -x "`command -v apt-get 2>/dev/null`" && { echo "setup requires apt-get. Install build-essential, linux-headers-`uname -r`, and gcc-12 manually."; exit 1; } || true
	sudo apt-get update
	sudo apt-get install -y build-essential linux-headers-`uname -r` gcc-12

install: check-linux hello_lkm.ko count_user_processes_lkm.ko
	@lsmod | grep -q '^hello_lkm\b' && { echo "hello_lkm is already loaded; unloading it first."; sudo rmmod hello_lkm; } || true
	sudo insmod hello_lkm.ko
	@lsmod | grep -q '^count_user_processes_lkm\b' && { echo "count_user_processes_lkm is already loaded; unloading it first."; sudo rmmod count_user_processes_lkm; } || true
	sudo insmod count_user_processes_lkm.ko
	@echo "Loaded hello_lkm and count_user_processes_lkm. Check dmesg with: sudo dmesg | tail"
	@echo "now run make test"

uninstall: check-linux
	sudo rmmod hello_lkm
	sudo rmmod count_user_processes_lkm

reload: install

test: install
	sudo dmesg | grep 'SRIDHAR .*hello_lkm' | tail -5
	sudo dmesg | grep 'SRIDHAR .*count_user_processes_lkm' | tail -5
	@echo "next thing to learn: bpftrace one-liners using kernel tracepoints (instead of kernel module)"

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
