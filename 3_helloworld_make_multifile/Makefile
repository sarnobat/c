# Format:
#
# target: dependencies
# 	system command(s)


# Make without any args executes "all"
all:
	helloworld
helloworld:
	helloworld.o helper.o
	gcc -o helloworld helloworld.o
helloworld.o:
	helloworld.c
	helper.c
	gcc -c helloworld.c helper.c
clean:
	rm helloworld.o 
	rm helper.o
	rm helloworld 
