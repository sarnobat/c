build:
	javac HelloWorld.java && \
	clang -I"`/usr/libexec/java_home`/include" \
	      -I"`/usr/libexec/java_home`/include/darwin" \
	      hello.c -o hello \
	      -L"`/usr/libexec/java_home`/lib/server" -ljvm

clean:
	rm -f hello
