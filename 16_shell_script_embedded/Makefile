DASH_DIR := /tmp/dash

build: init
	clang hello_dash.c -o hello_dash

init:
	test -d $(DASH_DIR) || git clone https://git.kernel.org/pub/scm/utils/dash/dash.git $(DASH_DIR)
	cd $(DASH_DIR) && autoreconf -fi && ./configure --disable-nls --without-libedit
	$(MAKE) -C $(DASH_DIR)


run: build
	./hello_dash

clean:
	$(MAKE) -C $(DASH_DIR) clean || true
	rm -f hello_dash
