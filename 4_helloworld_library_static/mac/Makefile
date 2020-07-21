# Format:
#
# target: dependencies
# 	system command(s)

# sudo port install json-c

# Make without any args executes "all"
all:	helloworld helloworld_static

helloworld: helloworld_obj
	# I believe this is dynamic linking, which I don't like.
	gcc -o helloworld_static 	helloworld.o -l json
	chown sarnobat helloworld_static

helloworld_static:	helloworld_obj
	gcc -o helloworld 		helloworld.o -l json -static
	rm helloworld.o

helloworld_obj:
	gcc -c helloworld.c

clean:
	rm -f helloworld.o helloworld a.out helloworld_static
