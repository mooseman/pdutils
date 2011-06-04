CFLAGS = -Wall -I/usr/include/db1
LDFLAGS = -g
CC = gcc

TARGETS = ladsh3 ladsh2 ladsh1 slinit slcharset sighup mknod dircontents \
	  globit mpx-blocks mpx-nonblock statsamp calc popenglob robin \
	  mpx-select map-cat ptytest lock dbsimple phones userver uclient \
	  lookup minopen services tserver tclient gather passfd id slecho \
	  ladsh4 monitor checkflags setflags usehello loadhello readpass cat \
	  match
SLANG = -lslang
POPT = -lpopt
DB = -ldb

all: $(TARGETS) 

slinit: slinit.c $(SLANG)

slcharset: slcharset.c $(SLANG)

slecho: slecho.c $(SLANG)

ptytest: ptytest.c ptypair.c

dbsimple: dbsimple.c $(DB)

phones: phones.c $(DB)

robin: robin.c $(POPT)

userver: userver.c sockutil.o

uclient: uclient.c sockutil.o

tserver: tserver.c sockutil.o

tclient: tclient.c sockutil.o

passfd: passfd.c sockutil.o

libhello.o: libhello.c
	@echo "*  Use -fPIC to build an object file for a shared library."
	gcc -fPIC $(CFLAGS) -c $<

libhello.so.0.0:	libhello.o
	@echo "*  Link libhello against libc for best results on all systems."
	gcc $(LDFLAGS) -shared -Wl,-soname,libhello.so.0 -o $@ $< -lc
	@echo "*  Create a pointer from the soname to the library."
	ln -sf libhello.so.0.0 libhello.so.0
	@echo "*  Create a pointer for the linker."
	ln -sf libhello.so.0 libhello.so

usehello: usehello.o libhello.so.0.0
	@echo "*  -L. -lhello, not libhello.so.0"
	gcc $(LDFLAGS) -o $@ $< -L. -lhello
	@echo "*  Now run: LD_LIBRARY_PATH=`pwd` ./$@"

loadhello: loadhello.o libhello.so.0.0
	gcc $(LDFLAGS) -o $@ $< -ldl
	@echo "Now run: LD_LIBRARY_PATH=`pwd` ./$@"

expand:
	@for n in *.c; do \
	    expand $$n > $$n.foo; \
	    if cmp -s $$n $$n.foo ; then \
		rm -f $$n.foo; \
	    else  \
		echo "Updating $$n"; \
		mv $$n.foo $$n; \
	    fi \
	done 

clean:
	rm -f $(TARGETS) *.o libhello.so*

