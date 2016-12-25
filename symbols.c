#include "stack.h"
#include "symbols.h"
#include "util.h"
#include "arithmetic.h"
#include "simplecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/*
*   Create a crude 'two-pass' symbol table system.
*   The first pass should collect all of the line numbers, constants and variables.
*   As constants and variables are collected they should be given
*   sequentially increasing addresses, starting from 1.
*   Line numbers should not be given addresses on the first pass.
*   The second pass will merely give the line numbers sequentially
*   increasing addresses, starting directly after the last
*   address given to a constant or variable.
*/
//The first entry of the symbol table is the length of the symbol table
int precedence(char *token){
	switch(*token){
		case '+': case '-':
		return 1;
		case '*': case '/':
		return 2;
		case '^':
		return 3;
		default:
        return 0;
	}
}

StkPtr createSymbolTable(StkPtr st){
    StkPtr symbols = newStk();
    SymPtr symptr;
    for(int i = 0; i < st->top; i++){
        if(precedence(*(st->stack + i)) == 0){
            symptr = (SymPtr)malloc(sizeof(Sym));
            symptr->symbol = (char *)malloc(strlen(st->stack[i]));
            strcpy(symptr->symbol, st->stack[i]);
            symptr->addr = symbols->top+1;
            symptr->type = isdigit(*(symptr->symbol))?'c':'v';
            push(symbols, symptr);
        }
    }
    return symbols;
}

void printCode(char *inputCode, StkPtr symbols){
    int numOfSymbols = 0;
    char *curLine, *nextLine, *lineNum, *instruction, *token, *leftMath, *op, *rightMath;
    StkPtr stkptr;

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
    lineNum = strtok(curLine, " ");
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
    while(curLine = strtok_r(nextLine, "\n", &nextLine)){
        lineNum = strtok(curLine, " ");
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

StkPtr secondPass(char *inputCode, StkPtr symbols){ //Error checking is done in first pass, so this pass is lazier
    char *curLine, *nextLine, *lineNum, *instruction, *token, *leftMath, *op, *rightMath, *codeCopy = (char *)malloc(strlen(inputCode));
    int offset = getHighestAddress(symbols) + 1;
    strcpy(codeCopy, inputCode);
    curLine = strtok_r(codeCopy, "\n", &nextLine);
    lineNum = strtok(curLine, " ");
    instruction = strtok(NULL, " ");
    updateSymbol(symbols, lineNum, 'l', offset);
    if(!strcmp(instruction, "goto")||!strcmp(instruction, "print")||!strcmp(instruction, "input")||!strcmp(instruction, "end")){
        offset++;
    }
    if(!strcmp(instruction, "let")){
        token = strtok(NULL, " ");
        op = strtok(NULL, " ");
        rightMath = strtok(NULL, " ");
        offset += 3*countOperators(rightMath); //There are 3 assembly instructions for each arithmetic operator
        offset += 2; //A load and a store operation are necessary at least
    }
    if(!strcmp(instruction, "if")){
        token = strtok(NULL, " ");
        leftMath = strtok(NULL, " ");
        op = strtok(NULL, " ");
        rightMath = strtok(NULL, " ");
        offset += 3*countOperators(leftMath); //There are 3 assembly instructions for each arithmetic operator
        offset += 3*countOperators(rightMath);
        offset += 3; //Plus three for the final difference of the left and right
        offset += 1; //Plus one for the branch instruction
        if(strcmp(op, "<=")==0||strcmp(op, ">=")==0){
            offset+=1; //An extra branch instruction is needed for these
        }
    }

    //First line done, begin looping through the rest
    while(curLine = strtok_r(nextLine, "\n", &nextLine)){
        lineNum = strtok(curLine, " ");
        instruction = strtok(NULL, " ");
        updateSymbol(symbols, lineNum, 'l', offset);

        if(!strcmp(instruction, "goto")||!strcmp(instruction, "print")||!strcmp(instruction, "input")||!strcmp(instruction, "end")){
            offset++;
        }
        if(!strcmp(instruction, "let")){
            token = strtok(NULL, " ");
            op = strtok(NULL, " ");
            rightMath = strtok(NULL, " ");
            offset += 3*countOperators(rightMath); //There are 3 assembly instructions for each arithmetic operator
            offset += 2; //A load and a store operation are necessary at least
        }
        if(!strcmp(instruction, "if")){
            token = strtok(NULL, " ");
            leftMath = strtok(NULL, " ");
            op = strtok(NULL, " ");
            rightMath = strtok(NULL, " ");
            offset += 3*countOperators(leftMath); //There are 3 assembly instructions for each arithmetic operator
            offset += 3*countOperators(rightMath);
            offset += 3; //Plus three for the final difference of the left and right
            offset += 1; //Plus one for the branch instruction
            if(strcmp(op, "<=")==0||strcmp(op, ">=")==0){
                offset+=1; //An extra branch instruction is needed for these
            }
        }
    }
    return symbols;
}

StkPtr firstPass(char *inputCode){
    char *codeCopy;
    StkPtr symbols = newStk();
    char *curLine, *nextLine;

    codeCopy = (char *)malloc(strlen(inputCode));
    strcpy(codeCopy, inputCode);

    curLine = strtok_r(codeCopy, "\n", &nextLine);
    parseLine(curLine, symbols);
    while(curLine = strtok_r(nextLine, "\n", &nextLine)){
        if(parseLine(curLine, symbols) == NULL)
            return NULL;
    }

    return symbols;
}

StkPtr parseLine(char *line, StkPtr symbols){
    char *lineNum, *instruction, *token, *leftMath, *op, *rightMath, *garbage, *stackValue;
    long *lineValuePtr;
    StkPtr stkptr;
    SymPtr symptr;

    lineNum = strtok(line, " ");
    lineValuePtr = parseLong(lineNum);
    if(lineValuePtr){
        if(*lineValuePtr <= highestLineNum(symbols)){
            printf("Non-increasing line number found: %ld\n", *lineValuePtr);
            return NULL;
        }
    }
    else{
        printf("Non-numeric line number: %s\n", lineNum);
        return NULL;
    }

    instruction = strtok(NULL, " ");
    if(instruction == NULL){
        return symbols;
    }
    if(strcmp(instruction, "rem")==0){
        if(lineDoesntExist(symbols, lineNum)){
            symptr = (SymPtr)malloc(sizeof(Sym));
            symptr->symbol = (char *)malloc(strlen(lineNum));
            strcpy(symptr->symbol, lineNum);
            symptr->addr = 0;
            symptr->type = 'l';
            push(symbols, symptr);
        }
        strtok(NULL, "\n"); //Clear out rest of line
        return symbols;
    }
    if(strcmp(instruction, "input")==0){
        token = strtok(NULL, " ");
        if(token){
            if(garbage = strtok(NULL, " ")){
                printf("Extra symbols (%s) detected on input statement!\n", garbage);
                return NULL;
            }
            else{
                if(variableDoesntExist(symbols, token)){
                    symptr = (SymPtr)malloc(sizeof(Sym));
                    symptr->symbol = (char *)malloc(strlen(token));
                    strcpy(symptr->symbol, token);
                    symptr->addr = getHighestAddress(symbols)+1;
                    symptr->type = 'v';
                    push(symbols, symptr);
                }
                if(lineDoesntExist(symbols, lineNum)){
                    symptr = (SymPtr)malloc(sizeof(Sym));
                    symptr->symbol = (char *)malloc(strlen(lineNum));
                    strcpy(symptr->symbol, lineNum);
                    symptr->addr = 0;
                    symptr->type = 'l';
                    push(symbols, symptr);
                }
                return symbols;
            }
        }
        else{
            printf("Variable missing after input statement!\n");
            return NULL;
        }
    }
    if(strcmp(instruction, "print")==0){
        token = strtok(NULL, " ");
        if(token){
            if(garbage = strtok(NULL, " ")){
                printf("Extra symbols (%s) detected after print statement!\n", garbage);
                return NULL;
            }
            else{
                if(variableDoesntExist(symbols, token)){
                    symptr = (SymPtr)malloc(sizeof(Sym));
                    symptr->symbol = (char *)malloc(strlen(token));
                    strcpy(symptr->symbol, token);
                    symptr->addr = getHighestAddress(symbols)+1;
                    symptr->type = 'v';
                    push(symbols, symptr);
                }
                if(lineDoesntExist(symbols, lineNum)){
                    symptr = (SymPtr)malloc(sizeof(Sym));
                    symptr->symbol = (char *)malloc(strlen(lineNum));
                    strcpy(symptr->symbol, lineNum);
                    symptr->addr = 0;
                    symptr->type = 'l';
                    push(symbols, symptr);
                }
                return symbols;
            }
        }
        else{
            printf("Variable missing after print statement!\n");
            return NULL;
        }
    }
    if(strcmp(instruction, "goto")==0){
        token = strtok(NULL, " ");
        if(token){
            if(garbage = strtok(NULL, " ")){
                printf("Extra symbols (%s) detected after goto statement!\n", garbage);
                return NULL;
            }
            else{
                /*lineValuePtr = parseLong(token);
                if(lineValuePtr){ //Checks if line number is actually even a number
                    if(lineDoesntExist(symbols, token)){
                        symptr = (SymPtr)malloc(sizeof(Sym));
                        symptr->symbol = (char *)malloc(strlen(token));
                        strcpy(symptr->symbol, token);
                        symptr->addr = 0;
                        symptr->type = 'l';
                        push(symbols, symptr);
                    }
                }
                else{
                    printf("Non-numeric line number: %s\n", token);
                    return NULL;
                }*/ //This line number will be added eventually anyway.
                    //And if it's not then this is an unresolved
                    //reference, which will be easy to spot now.
                lineValuePtr = parseLong(token); //Keep checking for proper input though
                if(!lineValuePtr){
                    printf("Non-numeric line number: %s\n", token);
                    return NULL;
                }
                if(lineDoesntExist(symbols, lineNum)){
                    symptr = (SymPtr)malloc(sizeof(Sym));
                    symptr->symbol = (char *)malloc(strlen(lineNum));
                    strcpy(symptr->symbol, lineNum);
                    symptr->addr = 0;
                    symptr->type = 'l';
                    push(symbols, symptr);
                }
                return symbols;
            }
        }
        else{
            printf("Line number missing after goto statement!\n");
            return NULL;
        }
    }
    if(strcmp(instruction, "let")==0){
        token = strtok(NULL, " ");
        if(token){
            op = strtok(NULL, " "); //op is = in this case
            if(op && strcmp(op, "=") == 0){
                rightMath = strtok(NULL, " ");
                if(rightMath){
                    stkptr = shunt(rightMath);
                    if(stkptr){
                        if(stackIsValid(stkptr)){

                            if(variableDoesntExist(symbols, token)){
                                symptr = (SymPtr)malloc(sizeof(Sym));
                                symptr->symbol = (char *)malloc(strlen(token));
                                strcpy(symptr->symbol, token);
                                symptr->addr = getHighestAddress(symbols)+1;
                                symptr->type = 'v';
                                push(symbols, symptr);
                            }

                            for(int i = 0; i < stkptr->top; i++){
                                stackValue = (char *)stkptr->stack[i];
                                if(isdigit(*stackValue)){ //Number
                                    if(constantDoesntExist(symbols, stackValue)){
                                        symptr = (SymPtr)malloc(sizeof(Sym));
                                        symptr->symbol = (char *)malloc(strlen(stackValue));
                                        strcpy(symptr->symbol, stackValue);
                                        symptr->addr = getHighestAddress(symbols)+1;
                                        symptr->type = 'c';
                                        push(symbols, symptr);
                                    }
                                }
                                else if(precedence(stackValue) == 0){ //Variable
                                    if(variableDoesntExist(symbols, stackValue)){
                                        symptr = (SymPtr)malloc(sizeof(Sym));
                                        symptr->symbol = (char *)malloc(strlen(stackValue));
                                        strcpy(symptr->symbol, stackValue);
                                        symptr->addr = getHighestAddress(symbols)+1;
                                        symptr->type = 'v';
                                        push(symbols, symptr);
                                    }
                                }
                            }

                            if(lineDoesntExist(symbols, lineNum)){
                                symptr = (SymPtr)malloc(sizeof(Sym));
                                symptr->symbol = (char *)malloc(strlen(lineNum));
                                strcpy(symptr->symbol, lineNum);
                                symptr->addr = 0;
                                symptr->type = 'l';
                                push(symbols, symptr);
                            }
                            return symbols;
                        }
                        else{
                            printf("Poorly formatted math: %s\n", rightMath);
                            return NULL;
                        }
                    }
                    else{
                        printf("Math could not be parsed: %s\n", rightMath);
                        return NULL;
                    }
                }
                else{
                    printf("Assignment missing after equals sign in let statement!\n");
                    return NULL;
                }
            }
            else{
                printf("Missing or invalid sign in let statement: %s\n", op);
                return NULL;
            }
        }
        else{
            printf("Variable missing after let statement!\n");
            return NULL;
        }
    }
    if(strcmp(instruction, "if")==0){
        leftMath = strtok(NULL, " ");
        if(leftMath){
            op = strtok(NULL, " ");
            if(op && (!strcmp(op, "==")||!strcmp(op, ">=")||!strcmp(op, "<=")||!strcmp(op, ">")||!strcmp(op, "<")) ){ //!strcmp actually means the strings are equal
                rightMath = strtok(NULL, " ");
                if(rightMath){
                    token = strtok(NULL, " ");  //This is a "goto" token
                    if(token && strcmp(token, "goto") == 0){
                        token = strtok(NULL, " ");  //This is now a line number
                        if(token){
                            stkptr = shunt(leftMath);
                            if(stkptr){
                                if(stackIsValid(stkptr)){
                                    for(int i = 0; i < stkptr->top; i++){
                                        stackValue = (char *)stkptr->stack[i];
                                        if(isdigit(*stackValue)){ //Number
                                            if(constantDoesntExist(symbols, stackValue)){
                                                symptr = (SymPtr)malloc(sizeof(Sym));
                                                symptr->symbol = (char *)malloc(strlen(stackValue));
                                                strcpy(symptr->symbol, stackValue);
                                                symptr->addr = getHighestAddress(symbols)+1;
                                                symptr->type = 'c';
                                                push(symbols, symptr);
                                            }
                                        }
                                        else if(precedence(stackValue) == 0){ //Variable
                                            if(variableDoesntExist(symbols, stackValue)){
                                                symptr = (SymPtr)malloc(sizeof(Sym));
                                                symptr->symbol = (char *)malloc(strlen(stackValue));
                                                strcpy(symptr->symbol, stackValue);
                                                symptr->addr = getHighestAddress(symbols)+1;
                                                symptr->type = 'v';
                                                push(symbols, symptr);
                                            }
                                        }
                                    }
                                }
                                else{
                                    printf("Poorly formatted math: %s\n", leftMath);
                                    return NULL;
                                }
                            }
                            else{
                                printf("Math could not be parsed: %s\n", rightMath);
                                return NULL;
                            }
                            stkptr = shunt(rightMath);
                            if(stkptr){
                                if(stackIsValid(stkptr)){
                                    for(int i = 0; i < stkptr->top; i++){
                                        stackValue = (char *)stkptr->stack[i];
                                        if(isdigit(*stackValue)){ //Number
                                            if(constantDoesntExist(symbols, stackValue)){
                                                symptr = (SymPtr)malloc(sizeof(Sym));
                                                symptr->symbol = (char *)malloc(strlen(stackValue));
                                                strcpy(symptr->symbol, stackValue);
                                                symptr->addr = getHighestAddress(symbols)+1;
                                                symptr->type = 'c';
                                                push(symbols, symptr);
                                            }
                                        }
                                        else if(precedence(stackValue) == 0){ //Variable
                                            if(variableDoesntExist(symbols, stackValue)){
                                                symptr = (SymPtr)malloc(sizeof(Sym));
                                                symptr->symbol = (char *)malloc(strlen(stackValue));
                                                strcpy(symptr->symbol, stackValue);
                                                symptr->addr = getHighestAddress(symbols)+1;
                                                symptr->type = 'v';
                                                push(symbols, symptr);
                                            }
                                        }
                                    }
                                }
                                else{
                                    printf("Poorly formatted math: %s\n", rightMath);
                                    return NULL;
                                }
                            }
                            else{
                                printf("Math could not be parsed: %s\n", rightMath);
                                return NULL;
                            }

                            lineValuePtr = parseLong(token); //Keep checking for proper input though
                            if(lineValuePtr){
                                if(garbage = strtok(NULL, " ")){
                                    printf("Extra symbols (%s) detected after goto statement!\n", garbage);
                                    return NULL;
                                }
                            }
                            else{
                                printf("Non-numeric line number: %s\n", token);
                                return NULL;
                            }
                            if(lineDoesntExist(symbols, lineNum)){
                                symptr = (SymPtr)malloc(sizeof(Sym));
                                symptr->symbol = (char *)malloc(strlen(lineNum));
                                strcpy(symptr->symbol, lineNum);
                                symptr->addr = 0;
                                symptr->type = 'l';
                                push(symbols, symptr);
                            }
                            return symbols;
                        }
                        else{
                            printf("Line number missing from if statement!\n");
                            return NULL;
                        }
                    }
                    else{
                        printf("Invalid expression in place of \"goto\": %s\n", token);
                        return NULL;
                    }
                }
                else{
                    printf("Right half of comparison missing from if statement!\n");
                    return NULL;
                }
            }
            else{
                printf("Missing or invalid comparison operator: %s\n", op);
            }
        }
        else{
            printf("Comparison expression missing from if statement!\n");
            return NULL;
        }
    }
    if(strcmp(instruction, "end")==0){
        if(garbage = strtok(NULL, " ")){
            printf("Extra symbols (%s) detected after end statement!\n", garbage);
            return NULL;
        }
        else if(lineDoesntExist(symbols, lineNum)){
            symptr = (SymPtr)malloc(sizeof(Sym));
            symptr->symbol = (char *)malloc(strlen(lineNum));
            strcpy(symptr->symbol, lineNum);
            symptr->addr = 0;
            symptr->type = 'l';
            push(symbols, symptr);
        }
        return symbols;
    }
}

//These are meant to be used on a StkPtr full of SymPtr's
int getHighestAddress(StkPtr stk){
    if(stk == NULL)
        return -1;

    int out = 0;
    for(int i = 0; i < stk->top; i++){
        if(((SymPtr)stk->stack[i])->addr > out)
            out = ((SymPtr)stk->stack[i])->addr;
    }
    return out;
}

int variableDoesntExist(StkPtr stk, char *token){
    if(stk == NULL)
        return -1;

    for(int i = 0; i < stk->top; i++){
        if(((SymPtr)stk->stack[i])->type == 'v')
        if(strcmp(((SymPtr)stk->stack[i])->symbol, token) == 0)
            return 0;
    }
    return 1;

}
int constantDoesntExist(StkPtr stk, char *token){
    if(stk == NULL)
        return -1;

    for(int i = 0; i < stk->top; i++){
        if(((SymPtr)stk->stack[i])->type == 'c')
        if(strcmp(((SymPtr)stk->stack[i])->symbol, token) == 0)
            return 0;
    }
    return 1;

}
int lineDoesntExist(StkPtr stk, char *token){
    if(stk == NULL)
        return -1;

    for(int i = 0; i < stk->top; i++){
        if(((SymPtr)stk->stack[i])->type == 'l')
        if(strcmp(((SymPtr)stk->stack[i])->symbol, token) == 0)
            return 0;
    }
    return 1;

}
int highestLineNum(StkPtr stk){
    if(stk == NULL)
        return -1;

    int highest = 0;
    long *test;
    for(int i = 0; i < stk->top; i++){
        if(((SymPtr)stk->stack[i])->type == 'l'){
            if(test = parseLong(((SymPtr)stk->stack[i])->symbol)){
                if((int)*test > highest){
                    highest = (int)*test;
                }
            }
            else{
                printf("Stack has non-numeric line number\n");
                return -1;
            }
        }
    }
    return highest;

}
int updateSymbol(StkPtr symbols, char * name, char type, int addr){
   SymPtr testSymbol;
   for(int i = 0; i < symbols->top; i++){
        testSymbol = ((SymPtr)symbols->stack[i]);
        if(strcmp(testSymbol->symbol, name) == 0){
            if(testSymbol->type == type){
                testSymbol->addr = addr;
                return 1;
            }
        }
    }
    return 0;
}

int countOperators(char *math){
    int count = 0;
    while(*math != '\0'){
        if(precedence(math))
            count++;
        math++;
    }
    return count;
}

int getSymbolAddress(StkPtr symbols, char *name, char type){
    SymPtr testSymbol;
    for(int i = 0; i < symbols->top; i++){
        testSymbol = ((SymPtr)symbols->stack[i]);
        if(strcmp(testSymbol->symbol, name) == 0){
            if(testSymbol->type == type){
                return testSymbol->addr;
            }
        }
    }
    return 0;
}

void mathToSimplecode(StkPtr symbols, char *leftMath, char *op, char *rightMath, char *branch){
    StkPtr leftStk = shunt(leftMath);
    StkPtr rightStk = shunt(rightMath);
    int scratchSpace = getHighestAddress(symbols) + 1;
    StkPtr tmp = newStk();
    long a, b, leftAddressNum, rightAddressNum, branchNum;
    char *a_st, *b_st, *leftAddress, *rightAddress;

    for(int i = 0; i < leftStk->top; i++){   //Replace the stack values with memory index labels
        if(precedence((char *)leftStk->stack[i]) == 0){
            if(isdigit(*(char *)leftStk->stack[i]))
                strcpy(leftStk->stack[i], longToString(getSymbolAddress(symbols, (char *)leftStk->stack[i], 'c')));
            else
                strcpy(leftStk->stack[i], longToString(getSymbolAddress(symbols, (char *)leftStk->stack[i], 'v')));
        }
    }
    for(int i = 0; i < rightStk->top; i++){   //Replace the stack values with memory index labels
        if(precedence((char *)rightStk->stack[i]) == 0){
            if(isdigit(*(char *)rightStk->stack[i]))
                strcpy(rightStk->stack[i], longToString(getSymbolAddress(symbols, (char *)rightStk->stack[i], 'c')));
            else
                strcpy(rightStk->stack[i], longToString(getSymbolAddress(symbols, (char *)rightStk->stack[i], 'v')));
        }
    }

    if(!strcmp(op, "=")){
        for(int i = 0; i < rightStk->top; i++){
            if(precedence(rightStk->stack[i]) == 0){
                push(tmp, rightStk->stack[i]);
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
                printf("%02d%02d, ", operatorToSimplecode(*(char *)rightStk->stack[i]), (int)b);
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
        leftAddress = pop(leftStk);
        rightAddress = pop(tmp);
        leftAddressNum = strtol(leftAddress, NULL, 10);
        rightAddressNum = strtol(rightAddress, NULL, 10);
        printf("%02d%02d, ", LOAD, (int)rightAddressNum);
        printf("%02d%02d, ", STORE, (int)leftAddressNum);
    }
    else{

        for(int i = 0; i < leftStk->top; i++){
            if(precedence(leftStk->stack[i]) == 0){
                push(tmp, leftStk->stack[i]);
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
                printf("%02d%02d, ", operatorToSimplecode(*(char *)leftStk->stack[i]), (int)b);
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
        leftAddress = pop(tmp);

        for(int i = 0; i < rightStk->top; i++){
            if(precedence(rightStk->stack[i]) == 0){
                push(tmp, rightStk->stack[i]);
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
                printf("%02d%02d, ", operatorToSimplecode(*(char *)rightStk->stack[i]), (int)b);
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
        rightAddress = pop(tmp);
        leftAddressNum = strtol(leftAddress, NULL, 10);
        rightAddressNum = strtol(rightAddress, NULL, 10);
        branchNum = getSymbolAddress(symbols, branch, 'l');

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
/*


    for(int i = 0; i < st->top; i++){
        if(precedence(st->stack[i]) == 0){
            push(tmp, st->stack[i]);
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
            printf("%02d%02d, ", operatorToSimplecode(*(char *)st->stack[i]), (int)b);
            //Store the result back into address a
            printf("%02d%02d, ", STORE, (int)a);
            //Put the address being stored into back on the address stack
            push(tmp, a_st);
        }
    }*/
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
    /*case '^':
        return 34;*/ //Not yet supported by simpletron
    default:
        printf("Bad symbol %c detected during parse!\n", a);
        return 0;
    }
}
