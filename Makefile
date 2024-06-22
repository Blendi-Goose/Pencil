compiler=gcc
compiler_flags=-O3 -lncurses
linker=gcc
linker_flags=-O3 -lncurses

all: main.o
	$(linker) $(linker_flags) main.o -o pencil

main.o: main.c
	$(compiler) $(compiler_flags) -c main.c -o main.o