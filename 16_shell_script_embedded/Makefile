DASH_DIR := dash


# Final binary (link all .o files including main.o)
hello: hello.c hello_sh.h $(DASH_DIR)/src/dash
	clang -I$(DASH_DIR)/src \
	      hello.c \
	      $(wildcard $(DASH_DIR)/src/*.o) \
	      $(wildcard $(DASH_DIR)/src/*/*.o) \
	      -o $@
	echo "Ironically, this is more complex than doing the equivalent in java or python"      


# Configure dash
$(DASH_DIR)/config.status:
	test -d $(DASH_DIR) || git clone https://git.kernel.org/pub/scm/utils/dash/dash.git $(DASH_DIR)
	cd $(DASH_DIR) && autoreconf -fi && ./configure --without-libedit

# Build dash
$(DASH_DIR)/src/dash: $(DASH_DIR)/config.status
	$(MAKE) -C $(DASH_DIR)

# hello.sh → hello_sh.h
hello_sh.h: hello.sh
	xxd -i hello.sh > $@

run: hello
	./hello

clean:
	$(MAKE) -C $(DASH_DIR) clean || true
	rm -f hello hello_sh.h
