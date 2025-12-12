# Format:
#
# target: dependencies
# 	system command(s)


# Make without any args executes "all"
all:
	helloworld
hello:
	helloworld.o
	gcc -o helloworld 			helloworld.o

#    Stage Selection Options

#        -E     Run the preprocessor stage.
#        -fsyntax-only
#               Run the preprocessor, parser and semantic analysis stages.
#        -S     Run the previous stages as well as LLVM generation and optimization stages and target-specific code generation, producing an assembly file.
#        -c     Run all of the above, plus the assembler, generating a target ".o" object file.
hello.o:
	hello.c
	gcc -c 						helloworld.c

# intermediate code - interesting for compilers academics
ir-mac:
	clang -S -emit-llvm 		helloworld.c

ir-linux:
	gcc 	-fdump-tree-gimple 	helloworld.c

clean:
	rm helloworld.o helloworld
