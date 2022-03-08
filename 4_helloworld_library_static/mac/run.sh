gcc -c helloworld.c 	-I/opt/local/include
gcc -o helloworld /opt/local/lib/libjson-c.dylib         -I/opt/local/lib        helloworld.o
echo "static linking not supported (or it's hard to do)"
# To debug:

# lldb ./helloworld
# b main
# run
# s