obj-m := hello_lkm.o
obj-m += count_user_processes_lkm.o
obj-m += proc_read_write_lkm.o

.PHONY: all build check-linux setup install uninstall reload test clean-intermediates clean

all: build clean-intermediates
	@echo "now run make install"

build: hello_lkm.ko count_user_processes_lkm.ko proc_read_write_lkm.ko

hello_lkm.ko: hello_lkm.c Makefile
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

count_user_processes_lkm.ko: count_user_processes_lkm.c Makefile
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

proc_read_write_lkm.ko: proc_read_write_lkm.c Makefile
	make -C /lib/modules/`uname -r`/build M=`pwd` modules

check-linux:
	@test "`uname -s`" != "Linux" && { echo "This module can only run on Linux, not `uname -s`."; exit 1; } || true

setup: check-linux
	@test ! -x "`command -v apt-get 2>/dev/null`" && { echo "setup requires apt-get. Install build-essential, linux-headers-`uname -r`, and gcc-12 manually."; exit 1; } || true
	sudo apt-get update
	sudo apt-get install -y build-essential linux-headers-`uname -r` gcc-12

install: check-linux hello_lkm.ko count_user_processes_lkm.ko proc_read_write_lkm.ko
	@lsmod | grep -q '^hello_lkm\b' && { echo "hello_lkm is already loaded; unloading it first."; sudo rmmod hello_lkm; } || true
	sudo insmod hello_lkm.ko
	@lsmod | grep -q '^count_user_processes_lkm\b' && { echo "count_user_processes_lkm is already loaded; unloading it first."; sudo rmmod count_user_processes_lkm; } || true
	sudo insmod count_user_processes_lkm.ko
	@lsmod | grep -q '^proc_read_write_lkm\b' && { echo "proc_read_write_lkm is already loaded; unloading it first."; sudo rmmod proc_read_write_lkm; } || true
	sudo insmod proc_read_write_lkm.ko
	@echo "Loaded hello_lkm, count_user_processes_lkm, and proc_read_write_lkm. Check dmesg with: sudo dmesg | tail"
	@echo "now run make test"

uninstall: check-linux
	sudo rmmod hello_lkm
	sudo rmmod count_user_processes_lkm
	sudo rmmod proc_read_write_lkm

reload: install

test: install
	sudo cat /proc/sridhar_lkm
	sudo sh -c 'echo hello_from_user_space > /proc/sridhar_lkm'
	sudo cat /proc/sridhar_lkm
	sudo dmesg | grep 'SRIDHAR .*hello_lkm' | tail -5
	sudo dmesg | grep 'SRIDHAR .*count_user_processes_lkm' | tail -5
	sudo dmesg | grep 'SRIDHAR .*proc_read_write_lkm' | tail -5
	@echo "next thing to learn: bpftrace one-liners using kernel tracepoints (instead of kernel module)"

clean-intermediates:
	rm -f hello_lkm.o hello_lkm.mod hello_lkm.mod.c hello_lkm.mod.o .hello_lkm.o.cmd .hello_lkm.mod.cmd .hello_lkm.mod.o.cmd .hello_lkm.ko.cmd
	rm -f count_user_processes_lkm.o count_user_processes_lkm.mod count_user_processes_lkm.mod.c count_user_processes_lkm.mod.o .count_user_processes_lkm.o.cmd .count_user_processes_lkm.mod.cmd .count_user_processes_lkm.mod.o.cmd .count_user_processes_lkm.ko.cmd
	rm -f proc_read_write_lkm.o proc_read_write_lkm.mod proc_read_write_lkm.mod.c proc_read_write_lkm.mod.o .proc_read_write_lkm.o.cmd .proc_read_write_lkm.mod.cmd .proc_read_write_lkm.mod.o.cmd .proc_read_write_lkm.ko.cmd
	rm -f Module.symvers modules.order .Module.symvers.cmd .modules.order.cmd

clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
