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

pencil: main.o
	$(linker) main.o $(linker_flags) -o pencil

main.o: main.c
	$(compiler) -c main.c $(compiler_flags) -o main.o

install: pencil
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 pencil $(DESTDIR)$(PREFIX)/bin/
