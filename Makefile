ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

ifeq ($(OS),Windows_NT)
	compiler=clang
else
	compiler=gcc
endif
compiler_flags=-O3 -lncurses

ifeq ($(OS),Windows_NT)
	linker=clang
else
	linker=gcc
endif
linker_flags=-O3 -lncurses

all: main.o
	$(linker) $(linker_flags) main.o -o pencil

main.o: main.c
	$(compiler) $(compiler_flags) -c main.c -o main.o

install: pencil
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 pencil $(DESTDIR)$(PREFIX)/bin/