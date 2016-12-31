.PHONY = all
CFLAGS := -std=gnu99 -g -Wall
all: simpletron scc

simpletron: stack.o longs.o arithmetic.o simplecode.h simpletron.c
	gcc -std=gnu99 -g -Wall stack.o longs.o arithmetic.o simpletron.c -o simpletron

scc: symbols.o longs.o arithmetic.o stack.o compiler.o scc.c
	gcc -std=gnu99 -g -Wall  stack.c symbols.c longs.c arithmetic.c compiler.c scc.c -o scc

stack.o: stack.c stack.h
	gcc -std=gnu99 -c stack.c -o stack.o

symbols.o: symbols.c symbols.h
	gcc -std=gnu99 -c symbols.c -o symbols.o

longs.o: longs.c longs.h
	gcc -std=gnu99 -c longs.c -o longs.o

arithmetic.o: arithmetic.c arithmetic.h
	gcc -std=gnu99 -c arithmetic.c -o arithmetic.o

compiler.o: compiler.c compiler.h
	gcc -std=gnu99 -c compiler.c -o compiler.o


