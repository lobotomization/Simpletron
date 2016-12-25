#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "simplecode.h"
#include "stack.h"
#include "symbols.h"
#include "arithmetic.h"
#include "util.h"


/*
* newStk - Returns a pointer to a new stack object
* pop - Returns the top-most string from the stack after removing it
* peek - Returns the top-most string from the stack without removing it
* compute - Applies the arithmetic operator specified by the char c
* power - Performs exponentiation of long ints
* parseRPN - This parses and evaluates a stack of arithmetic given in reverse polish notation
* longToString - Converts a long to a string for placement on a stack
* printStack - This prints the strings that currently reside on a stack object
* shunt -
*   This function uses the shunting yard algorithm.
*   This is where standard input is converted
*   into reverse polish notation.
*   It currently handles addition, multiplication
*   subtraction, division and exponentiation of
*   unsigned long ints.
* precedence - This returns the precedence of an arithmetic operator
* assoc - This returns the associativity of an operator (left or right)
* readToken - This parses the user input into numbers and operators
*/

void printStack(StkPtr st);
long compute(long a, long b, char c);
long power(long a, long b);
long *parseRPN(StkPtr st);


StkPtr copyStack(StkPtr st);
int main()
{

    char input[1024] = " 10 rem this is a comment\n 20 input x\n 30 print x\n 40 let y = x*x\n 50 let x = y \n 55 print y\n 60 if y < 10 goto 40\n 70 goto 80\n 80 end";
    /*
    StkPtr arithStack;
    long *sol;
    printf("Interactive Math Solver\n");
    do{
        printf("M> ");
        scanf("%s", input);
        if(strcmp(input, "quit") == 0)
            break;
        arithStack = shunt(input);
        if(arithStack){
            sol = parseRPN(arithStack);
            RPNToSimplecode(arithStack);
            printStack(arithStack);

        }
        if(sol)
        printf("%ld\n", *sol);

    }while(1);*/
    printf("%s\n\n********* Parsing Code *********\n", input);
    StkPtr symbolTable = firstPass(input);
    secondPass(input, symbolTable);
    printCode(input, symbolTable);
    if(symbolTable)
    for(int i = 0; i < symbolTable->top; i++){
        printf("i = %d\n", i);
        printf("Symbol: %s\n", ((SymPtr)symbolTable->stack[i])->symbol);
        printf("Address: %d\n", ((SymPtr)symbolTable->stack[i])->addr);
        printf("Type: %c\n", ((SymPtr)symbolTable->stack[i])->type);
    }
	return 0;
}


StkPtr copyStack(StkPtr st){
     char *newPointer;
     StkPtr newStack = newStk();
     for(int i = 0; i < st->top; i++){
        newPointer = (char *)malloc(strlen(st->stack[i]));
        strcpy(newPointer, st->stack[i]);
        push(newStack, newPointer);
     }
     return newStack;
}

/*void RPNToSimplecode(StkPtr st){
    StkPtr tmp = newStk(), cpy = copyStack(st);
    long a, b, c, indexLabel = 1, *out, numOfSymbols = 0;
    char *a_st, *b_st, *stackValues[st->top];

    StkPtr symbolTable = createSymbolTable(st);
    if(symbolTable)
        printf("Number of symbols is %d\n", symbolTable->top);
    while(numOfSymbols < symbolTable->top){
        printf("%s\n", ((SymPtr)*(symbolTable->stack+numOfSymbols))->symbol);
        numOfSymbols++;
    }
    printf("Number of symbols is %ld\n", numOfSymbols);

    for(int i = 0; i < cpy->top; i++){   //Replace the stack values with memory index labels
        //if(isdigit(**(st->stack + i))){
        if(precedence(*(cpy->stack + i)) == 0){
            stackValues[indexLabel - 1] = (char *)malloc(strlen(cpy->stack[i]));
            strcpy(stackValues[indexLabel - 1], cpy->stack[i]);
            strcpy(cpy->stack[i], longToString(indexLabel));
            indexLabel++;
        }
    }

    printf("{%02d%02d, ", BRANCH, (int)indexLabel); //Top of code. Branch instruction
    for(int i = 0; i < indexLabel-1; i++){    //Working stack
            if(isdigit(*stackValues[i]))
                printf("%s, ", stackValues[i]);
            else
                printf("0, ");
    }

    for(int i = 0; i < cpy->top; i++){
        if(precedence(cpy->stack[i]) == 0){
            push(tmp, cpy->stack[i]);
        }
        else{
            b_st = pop(tmp);   //Pop the strings backwards since this
            a_st = pop(tmp);  //is how they're stored on the stack
            if(a_st == NULL || b_st == NULL){
                printf("Inappropriate number of parameters!\n");
                break;
            }
            b = strtol(b_st, NULL, 10);
            a = strtol(a_st, NULL, 10);
            //Store value in address a in accumulator
            printf("%02d%02d, ", LOAD, (int)a);
            //Perform a (operator) b and store it in the accumulator
            printf("%02d%02d, ", operatorToSimplecode(*(char *)cpy->stack[i]), (int)b);
            //Store the result back into address a
            printf("%02d%02d, ", STORE, (int)a);
            //Put the address being stored into back on the address stack
            push(tmp, a_st);
        }
    }
    printf("%02d%02d, %02d%02d}\n", WRITE, (int)a, HALT, 0); //Display result and finish
}*/

long *parseRPN(StkPtr st){
    StkPtr tmp = newStk();
    long a, b, c, *out;
    char *a_st, *b_st, *cur;
    for(int i = 0; i < st->top; i++){
        cur = (char *)*(st->stack + i);
        if(isdigit(*cur)){
            push(tmp, cur);
        }
        else{
            b_st = pop(tmp);   //Pop the strings backwards since this
            a_st = pop(tmp);  //is how they're stored on the stack
            if(a_st == NULL || b_st == NULL){
                printf("Inappropriate number of parameters!\n");
                return NULL;
            }
            b = strtol(b_st, NULL, 10);
            a = strtol(a_st, NULL, 10);
            c = compute(a, b, *cur);
            push(tmp, longToString(c));
        }
    }

    out = malloc(sizeof(long));
    *out = strtol(pop(tmp), NULL, 10);

    return out;

}


long compute(long a, long b, char c){
    switch(c){
    case '+':
        return a+b;
    case '-':
        return a-b;
    case '*':
        return a*b;
    case '/':
        return a/b;
    case '^':
        return power(a,b);
    default:
        printf("Bad symbol %c detected during parse!\n", c);
        return 0;
    }
}

long power(long a, long b){
    long out = a;
    b--;
    while(b){
        out *= a;
        b--;
    }
    return out;
}




