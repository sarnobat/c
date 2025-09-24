ROOT	:=	/opt/homebrew/Cellar/

#VERSION 	:=	2.72.2
VERSION 	:=	2.84.4

#/opt/homebrew/Cellar/glib/2.84.4/include/glib-2.0/glib.h

all:	build 	run

build:
	gcc -I/opt/local/include/ `pkg-config --cflags glib-2.0 json-c` -c helloworld.c -o helloworld.o \
		&& gcc -o helloworldc `pkg-config --libs glib-2.0 json-c`  helloworld.o
		

run:
	find / | ./helloworldc | head