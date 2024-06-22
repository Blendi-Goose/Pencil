compiler=gcc
compiler_flags=-lncurses
linker=gcc
linker_flags=-lncurses

all: main.o
	$(linker) $(linker_flags) main.o -o pencil

main.o: main.c
	$(compiler) $(compiler_flags) -c main.c -o main.o