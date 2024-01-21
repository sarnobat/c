# Format:
#
# target: dependencies
# 	system command(s)


# Make without any args executes "all"
# why don't we need to mention helper.c explicitly?

helloworld: helloworld.o
	gcc -o helloworld helloworld.o
clean:
	rm *.o 
	rm helloworld
	