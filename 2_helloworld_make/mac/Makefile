# Format:
#
# target: dependencies
# 	system command(s)


# Make without any args executes "all"
all:	helloworld_o
helloworld_o:
	clang -c helloworld.c
	clang -o helloworld helloworld.o
	rm helloworld.o
clean:
	rm helloworld.o helloworld
