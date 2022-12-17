cat <<EOF
Symbols inside shared library (using nm)
----------------------------------------
EOF
nm libjson-c.dylib | head
cat <<EOF

Symbols inside shared library (using objdump)
---------------------------------------------
EOF
objdump -t libjson-c.dylib | head

cat <<EOF

           "B"
           "b" The symbol is in the BSS data section.  This section typically contains zero-initialized or uninitialized data, although the exact
               behavior is system dependent.
           "D"
           "d" The symbol is in the initialized data section.
           
           		variables
           
           "S"
           "s" The symbol is in an uninitialized or zero-initialized data section for small objects.
           
           		constants?
           		
           "T"
           "t" The symbol is in the text (code) section.
           
				functions
           
           "U" The symbol is undefined.
           
           		functions unused
EOF

exit

gcc -c helloworld.c 	-I/opt/local/include
gcc -o helloworld /opt/local/lib/libjson-c.dylib         -I/opt/local/lib        helloworld.o
echo "static linking not supported (or it's hard to do)"
# To debug:

# lldb ./helloworld
# b main
# run
# s