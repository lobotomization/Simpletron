CFLAGS := -std=gnu99 -g -Wall

clean: simpletron
	rm *.o

simpletron: arithtree
	gcc -std=gnu99 -g -Wall stack.c longs.c arithmetic.c simpletron.c -o simpletron

arithtree: symbols.o longs.o arithmetic.o stack.o compiler.o
	gcc -std=gnu99 -g -Wall  stack.c symbols.c longs.c arithmetic.c compiler.c arithtree.c -o arithtree
