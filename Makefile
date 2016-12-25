CFLAGS := -std=gnu99
arithtree: symbols.o util.o arithmetic.o stack.o
	gcc -std=gnu99 -g stack.c symbols.c util.c arithmetic.c arithtree.c -o arithtree
