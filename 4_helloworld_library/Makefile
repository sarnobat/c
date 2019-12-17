# Format:
#
# target: dependencies
# 	system command(s)


# Make without any args executes "all"
all:	helloworld

helloworld:	helloworld_obj
	# I believe this is dynamic linking, which I don't like.
	gcc -o helloworld helloworld.o -l json -static

helloworld_obj:
	gcc -c helloworld.c

clean:
	rm -f helloworld.o helloworld a.out
