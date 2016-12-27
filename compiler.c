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
void printCode(char *inputCode, StkPtr symbols){
    int numOfSymbols = 0;
    char *curLine, *nextLine, *instruction, *token, *leftMath, *op, *rightMath;

    for(int i = 0; i < symbols->top; i++){
        if(((SymPtr)symbols->stack[i])->type == 'c'||((SymPtr)symbols->stack[i])->type == 'v')
            numOfSymbols++;
    }

    printf("{%02d%02d, ", BRANCH, numOfSymbols + 1); //Top of code. Branch instruction

    for(int i = 0; i < symbols->top; i++){ //Constants and variables at top of code
        if(((SymPtr)symbols->stack[i])->type == 'c')
            printf("%s, ", ((SymPtr)symbols->stack[i])->symbol);
        if(((SymPtr)symbols->stack[i])->type == 'v')
            printf("0, ");
    }


    curLine = strtok_r(inputCode, "\n", &nextLine);
    strtok(curLine, " "); //Tokenize past line number
    instruction = strtok(NULL, " ");
    if(strcmp(instruction, "goto")==0){
        token = strtok(NULL, " ");
        printf("%02d%02d, ", BRANCH, getSymbolAddress(symbols, token, 'l'));
    }
    if(strcmp(instruction, "print")==0){
        token = strtok(NULL, " ");
        printf("%02d%02d, ", WRITE, getSymbolAddress(symbols, token, 'v'));
    }
    if(strcmp(instruction, "input")==0){
        token = strtok(NULL, " ");
        printf("%02d%02d, ", READ, getSymbolAddress(symbols, token, 'v'));
    }
    if(strcmp(instruction, "end")==0){
        printf("%02d%02d, ", HALT, 0);
    }
    if(strcmp(instruction, "let")==0){
        token = strtok(NULL, " ");
        op = strtok(NULL, " ");
        rightMath = strtok(NULL, " ");
        mathToSimplecode(symbols, token, op, rightMath, NULL);

    }
    if(strcmp(instruction, "if")==0){
        leftMath = strtok(NULL, " ");
        op = strtok(NULL, " ");
        rightMath = strtok(NULL, " ");
        strtok(NULL, " "); //Discard goto symbol
        token = strtok(NULL, " "); //Take line number
        mathToSimplecode(symbols, leftMath, op, rightMath, token);
    }

    //First line done, begin looping through the rest
    while((curLine = strtok_r(nextLine, "\n", &nextLine))){
        strtok(curLine, " "); //Tokenize past line number
        instruction = strtok(NULL, " ");
        if(strcmp(instruction, "goto")==0){
            token = strtok(NULL, " ");
            printf("%02d%02d, ", BRANCH, getSymbolAddress(symbols, token, 'l'));
        }
        if(strcmp(instruction, "print")==0){
            token = strtok(NULL, " ");
            printf("%02d%02d, ", WRITE, getSymbolAddress(symbols, token, 'v'));
        }
        if(strcmp(instruction, "input")==0){
            token = strtok(NULL, " ");
            printf("%02d%02d, ", READ, getSymbolAddress(symbols, token, 'v'));
        }
        if(strcmp(instruction, "end")==0){
            printf("%02d%02d, ", HALT, 0);
        }
        if(strcmp(instruction, "let")==0){
            token = strtok(NULL, " ");
            op = strtok(NULL, " ");
            rightMath = strtok(NULL, " ");
            mathToSimplecode(symbols, token, op, rightMath, NULL);

        }
        if(strcmp(instruction, "if")==0){
            leftMath = strtok(NULL, " ");
            op = strtok(NULL, " ");
            rightMath = strtok(NULL, " ");
            strtok(NULL, " "); //Discard goto symbol
            token = strtok(NULL, " "); //Take line number
            mathToSimplecode(symbols, leftMath, op, rightMath, token);
        }
    }
    printf("0}\n"); //At least pretend to close it off for now
}

void mathToSimplecode(StkPtr symbols, char *leftMath, char *op, char *rightMath, char *branch){
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
        stackToSimplecode(symbols, rightStk, tmp, scratchSpace);
        leftAddress = pop(leftStk);
        rightAddress = pop(tmp);
        leftAddressNum = strtol(leftAddress, NULL, 10);
        rightAddressNum = strtol(rightAddress, NULL, 10);
        printf("%02d%02d, ", LOAD, (int)rightAddressNum);
        printf("%02d%02d, ", STORE, (int)leftAddressNum);

        scratchSpace = getHighestAddress(symbols) + 1; //Reset scratch space after usage
    }
    else{
        //Make sure to keep the updated scratchSpace value for the next stack!!
        scratchSpace = stackToSimplecode(symbols, leftStk, tmp, scratchSpace);
        leftAddress = pop(tmp);
        stackToSimplecode(symbols, rightStk, tmp, scratchSpace);
        rightAddress = pop(tmp);
        leftAddressNum = strtol(leftAddress, NULL, 10);
        rightAddressNum = strtol(rightAddress, NULL, 10);
        branchNum = getSymbolAddress(symbols, branch, 'l');

        scratchSpace = getHighestAddress(symbols) + 1; //Reset scratch space after usage

    }
    if(!strcmp(op, "==")){
        printf("%02d%02d, ", LOAD, (int)rightAddressNum);
        printf("%02d%02d, ", SUBTRACT, (int)leftAddressNum);
        printf("%02d%02d, ", BRANCHZERO, (int)branchNum);
    }
    if(!strcmp(op, ">=")){
        printf("%02d%02d, ", LOAD, (int)rightAddressNum);
        printf("%02d%02d, ", SUBTRACT, (int)leftAddressNum);
        printf("%02d%02d, ", BRANCHZERO, (int)branchNum);
        printf("%02d%02d, ", BRANCHNEG, (int)branchNum);

    }
    if(!strcmp(op, "<=")){
        printf("%02d%02d, ", LOAD, (int)leftAddressNum);
        printf("%02d%02d, ", SUBTRACT, (int)rightAddressNum);
        printf("%02d%02d, ", BRANCHZERO, (int)branchNum);
        printf("%02d%02d, ", BRANCHNEG, (int)branchNum);
    }
    if(!strcmp(op, ">")){
        printf("%02d%02d, ", LOAD, (int)rightAddressNum);
        printf("%02d%02d, ", SUBTRACT, (int)leftAddressNum);
        printf("%02d%02d, ", BRANCHNEG, (int)branchNum);
    }
    if(!strcmp(op, "<")){
        printf("%02d%02d, ", LOAD, (int)leftAddressNum);
        printf("%02d%02d, ", SUBTRACT, (int)rightAddressNum);
        printf("%02d%02d, ", BRANCHNEG, (int)branchNum);
    }
    if(!strcmp(op, "!=")){
        printf("%02d%02d, ", LOAD, (int)leftAddressNum);
        printf("%02d%02d, ", SUBTRACT, (int)rightAddressNum);
        printf("%02d%02d, ", BRANCHNEG, (int)branchNum);
        printf("%02d%02d, ", LOAD, (int)rightAddressNum);
        printf("%02d%02d, ", SUBTRACT, (int)leftAddressNum);
        printf("%02d%02d, ", BRANCHNEG, (int)branchNum);
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

int stackToSimplecode(StkPtr symbols, StkPtr stk, StkPtr tmp, int scratchSpace){
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
            printf("%02d%02d, ", LOAD, (int)a);
            //Perform a (operator) b and store it in the accumulator
            printf("%02d%02d, ", operatorToSimplecode(*(char *)stk->stack[i]), (int)b);
            //Store the result into stratch space, or a or b if they're in scratch space already.
            if(a > getHighestAddress(symbols)){
                printf("%02d%02d, ", STORE, (int)a);
                push(tmp, a_st);
            }
            else if(b > getHighestAddress(symbols)){
                printf("%02d%02d, ", STORE, (int)b);
                push(tmp, b_st);
            }
            else{
                printf("%02d%02d, ", STORE, scratchSpace);
                push(tmp, longToString((long)scratchSpace));
                scratchSpace++;
            }
        }
    }
    return scratchSpace;
}
