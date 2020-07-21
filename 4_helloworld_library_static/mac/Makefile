# Format:
#
# target: dependencies
# 	system command(s)

# sudo port install json-c

# Make without any args executes "all"
all:	helloworld

helloworld: helloworld_obj
	# this is dynamic linking, which I don't like.
	gcc -o helloworld /opt/local/lib/libjson-c.dylib         -I/opt/local/lib        helloworld.o
	chown sarnobat helloworld
	rm helloworld.o

helloworld_static:	helloworld_obj
	echo "not supported"

helloworld_obj:
	gcc -c helloworld.c 	-I/opt/local/include
clean:
	rm -f helloworld.o helloworld a.out
