# Format:
#
# target: dependencies
# 	system command(s)


# Make without any args executes "all"
all:
	helloworld
hello:
	helloworld.o
	gcc -o helloworld helloworld.o
hello.o:
	hello.c
	gcc -c helloworld.c

clean:
	rm helloworld.o helloworld
