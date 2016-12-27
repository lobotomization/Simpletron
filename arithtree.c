#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "longs.h"
#include "simplecode.h"
#include "stack.h"
#include "symbols.h"
#include "arithmetic.h"
#include "compiler.h"
int main()
{
    //char input[1024] = " 10 rem this is a comment, and a meaningless program\n 20 input x\n 30 print x\n 40 let y = x*x\n 50 let x = y \n 55 print y\n 60 if y < 10 goto 40\n 70 goto 80\n 80 end";

    /*char input[1024] =\
     "  10 rem This calculates x mod y \n\
        20 input x \n\
        30 input y \n\
        40 let z = x/y \n\
        50 let r = (x-z*y) \n\
        55 print r \n\
        60 end";*/

    /*char input[1024] =\
    " 10 rem This calculates the divisors of x \n\
      20 input x \n\
      30 let y = 1 \n\
      40 let z = x/y \n\
      50 let r = (x-z*y) \n\
      60 if r > 0 goto 80 \n\
      70 if r == 0 goto 110 \n\
      80 let y = y+1 \n\
      90 if y > x goto 150 \n\
      100 goto 40 \n\
      110 print y \n\
      120 let y = y+1 \n\
      130 if y > x goto 150 \n\
      140 goto 40 \n\
      150 end ";*/

      //p is the potential prime, d is the number of divisors, y is a test divisor, q is the integer quotient of p and y
    /*char input[1024] =\
    " 10 rem This calculates the primes up to 75 \n\
      20 let p = 0 \n\
      25 let d = 0 \n\
      26 let p = p+1 \n\
      30 let y = 1 \n\
      40 let q = p/y \n\
      50 let r = (p-q*y) \n\
      60 if r > 0 goto 80 \n\
      70 if r == 0 goto 110 \n\
      80 let y = y+1 \n\
      90 if y > p goto 150 \n\
      100 goto 40 \n\
      110 let d = d+1 \n\
      120 let y = y+1 \n\
      130 if y > p goto 150 \n\
      140 goto 40 \n\
      150 if d != 2 goto 170 \n\
      160 print p \n\
      170 if p < 75 goto 25 \n\
      180 end ";*/

    /*char input[1024] =\
    "   10 rem Testing the != functionality \n\
        20 let p = 1 \n\
        30 let q = 2 \n\
        40 if p != q goto 60 \n\
        50 print p \n\
        60 print q \n\
        70 end ";*/

    /*char input[1024] =\
    "   10 rem Testing scratchspace functionality \n\
        20 let p = 1 \n\
        30 let q = 2 \n\
        40 let r = 3 \n\
        50 let s = 4 \n\
        60 let p = (p+q)*(r+s) \n\
        70 print q \n\
        80 end ";*/

    char input[1024] =\
    "   10 rem Testing exponentiation \n\
        20 let p = 1 \n\
        30 let q = 2 \n\
        40 let r = 3 \n\
        50 let s = 4 \n\
        60 let p = r*q^s \n\
        70 print p \n\
        80 end ";

    printf("%s\n\n********* Parsing Code *********\n", input);
    StkPtr symbolTable = firstPass(input);
    secondPass(input, symbolTable);
    printCode(input, symbolTable);
    /*if(symbolTable)
    for(int i = 0; i < symbolTable->top; i++){
        printf("i = %d\n", i);
        printf("Symbol: %s\n", ((SymPtr)symbolTable->stack[i])->symbol);
        printf("Address: %d\n", ((SymPtr)symbolTable->stack[i])->addr);
        printf("Type: %c\n", ((SymPtr)symbolTable->stack[i])->type);
    }*/
	return 0;
}
