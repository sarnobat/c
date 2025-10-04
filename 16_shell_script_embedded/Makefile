DASH_DIR := dash

# 1) Fetch & build dash (once)
init:
	test -d $(DASH_DIR) || git clone https://git.kernel.org/pub/scm/utils/dash/dash.git $(DASH_DIR)
	cd $(DASH_DIR) && autoreconf -fi && ./configure --without-libedit
	$$(MAKE) -C $(DASH_DIR)

# 2) Turn myscript.sh -> C header with a byte array (needs `xxd`, usually present on macOS)
script.inc.h: myscript.sh
	xxd -i myscript.sh > $@

# 3) Rename dash's 'main' symbol so we can define our own main()
#    Requires llvm-objcopy (part of Xcode CLT / Homebrew llvm)
$(DASH_DIR)/src/main.renamed.o: $(DASH_DIR)/src/main.o
	llvm-objcopy --redefine-symbol main=dash_main $< $@

# 4) Link everything into ONE binary (no runtime deps on dash or the script file)
build: init script.inc.h $(DASH_DIR)/src/main.renamed.o
	clang -I$(DASH_DIR)/src \
	      hello_dash.c \
	      $(filter-out $(DASH_DIR)/src/main.o,$(wildcard $(DASH_DIR)/src/*.o)) \
	      $(DASH_DIR)/src/main.renamed.o \
	      -o hello_dash

run: build
	./hello_dash

clean:
	$$(MAKE) -C $(DASH_DIR) clean || true
	rm -f hello_dash script.inc.h
