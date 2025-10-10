build:
	gcc -o hashgnu hashgnu.c `pkg-config --cflags --libs glib-2.0`
	gcc -o hashposix hashposix.c
	gcc listgnu.c -o listgnu

run:
	./hashposix
	./listgnu
	./hashgnu	