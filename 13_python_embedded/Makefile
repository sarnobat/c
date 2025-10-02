PFX       := /tmp/pyout
PYMAJMIN  := 3.12
INCDIR    := $(PFX)/include/python$(PYMAJMIN)
LIBA      := $(PFX)/lib/libpython$(PYMAJMIN).a
PYCONFIG  := $(PFX)/bin/python$(PYMAJMIN)-config


all: hello_embedded

hello_embedded: main.c hello_py.h
	clang -std=c11 -O2 \
	      -mmacosx-version-min=14.7 \
	      -I$(INCDIR) \
	      main.c $(LIBA) \
	      $$($(PYCONFIG) --embed --ldflags) \
	      -mmacosx-version-min=14.7 \
	      -o $@

hello_py.h: hello.py
	xxd -i $< > $@



interpreter:	init
	(test -d /tmp/pyout/ && exit 0) \
	|| { cd /tmp/Python-3.12.5/ \
	&& ./configure \
	  --prefix="/tmp/pyout/" \
	  --disable-shared \
	  --without-ensurepip \
	  --enable-optimizations \
	&& make \
	&& make install ; }\
	&& find /tmp/Python-3.12.5/libpython3.12.a
	
init:
	cd /tmp/ \
	&& { ls Python-3.12.5.tgz || curl -LO https://www.python.org/ftp/python/3.12.5/Python-3.12.5.tgz ; } \
	&& { test -d Python-3.12.5 || tar xf Python-3.12.5.tgz ; } \
	&& cd Python-3.12.5 && ls -lrt \
	&& open . \
	&& pwd