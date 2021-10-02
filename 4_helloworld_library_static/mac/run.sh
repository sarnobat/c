gcc -c helloworld.c 	-I/opt/local/include
gcc -o helloworld /opt/local/lib/libjson-c.dylib         -I/opt/local/lib        helloworld.o

# To debug:

# lldb ./helloworld
# b main
# run
# s