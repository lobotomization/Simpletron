CFLAGS := -std=gnu99 -g -Wall

simpletron: simpletron.o scc
	gcc -std=gnu99 -g -Wall stack.c longs.c arithmetic.c simpletron.c -o simpletron

scc: symbols.o longs.o arithmetic.o stack.o compiler.o
	gcc -std=gnu99 -g -Wall  stack.c symbols.c longs.c arithmetic.c compiler.c scc.c -o scc
