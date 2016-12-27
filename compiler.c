#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "stack.h"
#include "symbols.h"
#include "longs.h"
#include "simplecode.h"
#include "arithmetic.h"
#include "compiler.h"

void printCode(char *inputCode, StkPtr symbols, FILE* outfp){
    int numOfSymbols = 0;
    char *curLine, *nextLine;

    for(int i = 0; i < symbols->top; i++){
        if(((SymPtr)symbols->stack[i])->type == 'c'||((SymPtr)symbols->stack[i])->type == 'v')
            numOfSymbols++;
    }

    fprintf(outfp, "%02d%02d\n", BRANCH, numOfSymbols + 1); //Top of code. Branch instruction

    for(int i = 0; i < symbols->top; i++){ //Constants and variables at top of code
        if(((SymPtr)symbols->stack[i])->type == 'c')
            fprintf(outfp, "%s\n", ((SymPtr)symbols->stack[i])->symbol);
        if(((SymPtr)symbols->stack[i])->type == 'v')
            fprintf(outfp, "0\n");
    }


    curLine = strtok_r(inputCode, "\n\r", &nextLine);
    printCodeLine(symbols, curLine, outfp);

    //First line done, begin looping through the rest
    while((curLine = strtok_r(nextLine, "\n\r", &nextLine))){
        printCodeLine(symbols, curLine, outfp);
    }
    //printf("0}\n"); //At least pretend to close it off for now
}
void printCodeLine(StkPtr symbols, char *curLine, FILE *outfp){
    strtok(curLine, " "); //Tokenize past line number
    char *instruction = strtok(NULL, " "), *token, *leftMath, *op, *rightMath;
    if(strcmp(instruction, "goto")==0){
        token = strtok(NULL, " ");
        fprintf(outfp, "%02d%02d\n", BRANCH, getSymbolAddress(symbols, token, 'l'));
    }
    if(strcmp(instruction, "print")==0){
        token = strtok(NULL, " ");
       fprintf(outfp, "%02d%02d\n", WRITE, getSymbolAddress(symbols, token, 'v'));
    }
    if(strcmp(instruction, "input")==0){
        token = strtok(NULL, " ");
        fprintf(outfp, "%02d%02d\n", READ, getSymbolAddress(symbols, token, 'v'));
    }
    if(strcmp(instruction, "end")==0){
        fprintf(outfp, "%02d%02d\n", HALT, 0);
    }
    if(strcmp(instruction, "let")==0){
        token = strtok(NULL, " ");
        op = strtok(NULL, " ");
        rightMath = strtok(NULL, " ");
        mathToSimplecode(symbols, token, op, rightMath, NULL, outfp);

    }
    if(strcmp(instruction, "if")==0){
        leftMath = strtok(NULL, " ");
        op = strtok(NULL, " ");
        rightMath = strtok(NULL, " ");
        strtok(NULL, " "); //Discard goto symbol
        token = strtok(NULL, " "); //Take line number
        mathToSimplecode(symbols, leftMath, op, rightMath, token, outfp);
    }
}
void mathToSimplecode(StkPtr symbols, char *leftMath, char *op, char *rightMath, char *branch, FILE *outfp){
    StkPtr leftStk = shunt(leftMath);
    StkPtr rightStk = shunt(rightMath);
    int scratchSpace = getHighestAddress(symbols) + 1;
    StkPtr tmp = newStk();
    long leftAddressNum, rightAddressNum, branchNum;
    char *leftAddress, *rightAddress, *stackValue;

    for(int i = 0; i < leftStk->top; i++){   //Replace the stack values with memory index labels
        stackValue = (char *)leftStk->stack[i];
        if(precedence(stackValue) == 0){
            if(isdigit(*stackValue))
                strcpy(stackValue, longToString(getSymbolAddress(symbols, stackValue, 'c')));
            else
                strcpy(stackValue, longToString(getSymbolAddress(symbols, stackValue, 'v')));
        }
    }
    for(int i = 0; i < rightStk->top; i++){   //Replace the stack values with memory index labels
        stackValue = (char *)rightStk->stack[i];
        if(precedence(stackValue) == 0){
            if(isdigit(*stackValue))
                strcpy(stackValue, longToString(getSymbolAddress(symbols, stackValue, 'c')));
            else
                strcpy(stackValue, longToString(getSymbolAddress(symbols, stackValue, 'v')));
        }
    }

    if(!strcmp(op, "=")){
        stackToSimplecode(symbols, rightStk, tmp, scratchSpace, outfp);
        leftAddress = pop(leftStk);
        rightAddress = pop(tmp);
        leftAddressNum = strtol(leftAddress, NULL, 10);
        rightAddressNum = strtol(rightAddress, NULL, 10);
        fprintf(outfp, "%02d%02d\n", LOAD, (int)rightAddressNum);
        fprintf(outfp, "%02d%02d\n", STORE, (int)leftAddressNum);

        scratchSpace = getHighestAddress(symbols) + 1; //Reset scratch space after usage
    }
    else{
        //Make sure to keep the updated scratchSpace value for the next stack!!
        scratchSpace = stackToSimplecode(symbols, leftStk, tmp, scratchSpace, outfp);
        leftAddress = pop(tmp);
        stackToSimplecode(symbols, rightStk, tmp, scratchSpace, outfp);
        rightAddress = pop(tmp);
        leftAddressNum = strtol(leftAddress, NULL, 10);
        rightAddressNum = strtol(rightAddress, NULL, 10);
        branchNum = getSymbolAddress(symbols, branch, 'l');

        scratchSpace = getHighestAddress(symbols) + 1; //Reset scratch space after usage

    }
    if(!strcmp(op, "==")){
        fprintf(outfp, "%02d%02d\n", LOAD, (int)rightAddressNum);
        fprintf(outfp, "%02d%02d\n", SUBTRACT, (int)leftAddressNum);
        fprintf(outfp, "%02d%02d\n", BRANCHZERO, (int)branchNum);
    }
    if(!strcmp(op, ">=")){
        fprintf(outfp, "%02d%02d\n", LOAD, (int)rightAddressNum);
        fprintf(outfp, "%02d%02d\n", SUBTRACT, (int)leftAddressNum);
        fprintf(outfp, "%02d%02d\n", BRANCHZERO, (int)branchNum);
        fprintf(outfp, "%02d%02d\n", BRANCHNEG, (int)branchNum);

    }
    if(!strcmp(op, "<=")){
        fprintf(outfp, "%02d%02d\n", LOAD, (int)leftAddressNum);
        fprintf(outfp, "%02d%02d\n", SUBTRACT, (int)rightAddressNum);
        fprintf(outfp, "%02d%02d\n", BRANCHZERO, (int)branchNum);
        fprintf(outfp, "%02d%02d\n", BRANCHNEG, (int)branchNum);
    }
    if(!strcmp(op, ">")){
        fprintf(outfp, "%02d%02d\n", LOAD, (int)rightAddressNum);
        fprintf(outfp, "%02d%02d\n", SUBTRACT, (int)leftAddressNum);
        fprintf(outfp, "%02d%02d\n", BRANCHNEG, (int)branchNum);
    }
    if(!strcmp(op, "<")){
        fprintf(outfp, "%02d%02d\n", LOAD, (int)leftAddressNum);
        fprintf(outfp, "%02d%02d\n", SUBTRACT, (int)rightAddressNum);
        fprintf(outfp, "%02d%02d\n", BRANCHNEG, (int)branchNum);
    }
    if(!strcmp(op, "!=")){
        fprintf(outfp, "%02d%02d\n", LOAD, (int)leftAddressNum);
        fprintf(outfp, "%02d%02d\n", SUBTRACT, (int)rightAddressNum);
        fprintf(outfp, "%02d%02d\n", BRANCHNEG, (int)branchNum);
        fprintf(outfp, "%02d%02d\n", LOAD, (int)rightAddressNum);
        fprintf(outfp, "%02d%02d\n", SUBTRACT, (int)leftAddressNum);
        fprintf(outfp, "%02d%02d\n", BRANCHNEG, (int)branchNum);
    }
}

int operatorToSimplecode(char a){
    switch(a){
    case '+':
        return 30;
    case '-':
        return 31;
    case '/':
        return 32;
    case '*':
        return 33;
    case '^':
        return 34;
    default:
        printf("Bad symbol %c detected during parse!\n", a);
        return 0;
    }
}

int stackToSimplecode(StkPtr symbols, StkPtr stk, StkPtr tmp, int scratchSpace, FILE *outfp){
    char *a_st, *b_st;
    long a, b;
    for(int i = 0; i < stk->top; i++){
        if(precedence(stk->stack[i]) == 0){
            push(tmp, stk->stack[i]);
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
            fprintf(outfp, "%02d%02d\n", LOAD, (int)a);
            //Perform a (operator) b and store it in the accumulator
            fprintf(outfp, "%02d%02d\n", operatorToSimplecode(*(char *)stk->stack[i]), (int)b);
            //Store the result into stratch space, or a or b if they're in scratch space already.
            if(a > getHighestAddress(symbols)){
                fprintf(outfp, "%02d%02d\n", STORE, (int)a);
                push(tmp, a_st);
            }
            else if(b > getHighestAddress(symbols)){
                fprintf(outfp, "%02d%02d\n", STORE, (int)b);
                push(tmp, b_st);
            }
            else{
                fprintf(outfp, "%02d%02d\n", STORE, scratchSpace);
                push(tmp, longToString((long)scratchSpace));
                scratchSpace++;
            }
        }
    }
    return scratchSpace;
}
