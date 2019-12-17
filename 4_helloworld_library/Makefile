# Format:
#
# target: dependencies
# 	system command(s)


# Make without any args executes "all"
all:	helloworld

helloworld:	helloworld_obj
	gcc -o helloworld helloworld.o -l json

helloworld_obj:
	gcc -c helloworld.c -l json

clean:
	rm helloworld.o helloworld a.out
