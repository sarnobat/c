# Format:
#
# target: dependencies
# 	system command(s)


# Make without any args executes "all"
all:
	helloworld
hello:
	helloworld.o
	gcc -o helloworld helloworld.o -l json
hello.o:
	hello.c
	gcc -c helloworld.c -l json

clean:
	rm helloworld.o helloworld
