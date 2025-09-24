ROOT	:=	/opt/homebrew/Cellar/

#VERSION 	:=	2.72.2
VERSION 	:=	2.84.4

#/opt/homebrew/Cellar/glib/2.84.4/include/glib-2.0/glib.h

build:
	gcc -I/opt/local/include/ -I/opt/homebrew/Cellar/json-c/0.18/include/ -I\$(ROOT)/glib/\$(VERSION)/lib/glib-2.0/include/ -I\$(ROOT)/glib/\$(VERSION)/include/glib-2.0/ -c helloworld.c -o helloworld.o \
		&& gcc -o helloworldc /opt/homebrew/Cellar/json-c/0.18/lib/libjson-c.dylib  helloworld.o
		

run:
	find | ./helloworldc