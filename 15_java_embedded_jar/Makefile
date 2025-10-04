build:
	javac HelloWorld.java && \
	jar cfe HelloWorld.jar HelloWorld HelloWorld.class && \
	clang -I"`/usr/libexec/java_home`/include" \
	      -I"`/usr/libexec/java_home`/include/darwin" \
	      hello.c -o hello \
	      -L"`/usr/libexec/java_home`/lib/server" -ljvm

run: build
	./hello

clean:
	rm -f hello HelloWorld.class HelloWorld.jar
