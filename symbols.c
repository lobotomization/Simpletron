#include "stack.h"
#include "symbols.h"
#include "longs.h"
#include "arithmetic.h"
#include "simplecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


StkPtr secondPass(char *inputCode, StkPtr symbols){ //Error checking is done in first pass, so this pass is lazier
    char *curLine, *nextLine, *codeCopy = (char *)malloc(strlen(inputCode));
    int offset = getHighestAddress(symbols) + 1;
    strcpy(codeCopy, inputCode);
    curLine = strtok_r(codeCopy, "\n", &nextLine);
    offset = calculateLineNumbers(symbols, curLine, offset);

    //First line done, begin looping through the rest
    while((curLine = strtok_r(nextLine, "\n", &nextLine))){
        offset = calculateLineNumbers(symbols, curLine, offset);
    }
    return symbols;
}

StkPtr firstPass(char *inputCode){
    char *codeCopy;
    StkPtr symbols = newStk();
    char *curLine, *nextLine;

    codeCopy = (char *)malloc(strlen(inputCode));
    strcpy(codeCopy, inputCode);

    curLine = strtok_r(codeCopy, "\r\n", &nextLine);
    parseLine(curLine, symbols);
    while((curLine = strtok_r(nextLine, "\r\n", &nextLine))){
        if(parseLine(curLine, symbols) == NULL)
            return NULL;
    }

    return symbols;
}

StkPtr parseLine(char *line, StkPtr symbols){
    char *lineNum, *instruction, *token, *leftMath, *op, *rightMath, *garbage;
    long *lineValuePtr;

    lineNum = strtok(line, " ");
    lineValuePtr = parseLong(lineNum);
    if(lineValuePtr){
        if(*lineValuePtr <= getHighestLineNum(symbols)){
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
        addSymbolToTable(symbols, lineNum, 0, 'l');
        strtok(NULL, "\n"); //Clear out rest of line
        return symbols;
    }
    if(strcmp(instruction, "input")==0){
        token = strtok(NULL, " ");
        if(token){
            if((garbage = strtok(NULL, " "))){
                printf("Extra symbols (%s) detected on input statement!\n", garbage);
                return NULL;
            }
            else{
                addSymbolToTable(symbols, token, getHighestAddress(symbols)+1, 'v');
                addSymbolToTable(symbols, lineNum, 0, 'l');
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
            if((garbage = strtok(NULL, " "))){
                printf("Extra symbols (%s) detected after print statement!\n", garbage);
                return NULL;
            }
            else{
                addSymbolToTable(symbols, token, getHighestAddress(symbols)+1, 'v');
                addSymbolToTable(symbols, lineNum, 0, 'l');
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
            if((garbage = strtok(NULL, " "))){
                printf("Extra symbols (%s) detected after goto statement!\n", garbage);
                return NULL;
            }
            else{
                lineValuePtr = parseLong(token); //Check for proper input
                if(!lineValuePtr){
                    printf("Non-numeric line number: %s\n", token);
                    return NULL;
                }
                addSymbolToTable(symbols, lineNum, 0, 'l');
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
                        //Add variable to symbol table
                    addSymbolToTable(symbols, token, getHighestAddress(symbols)+1, 'v');
                        //Math is placed into symbol table here
                        //Function returns 1 on success, 0 on failure
                    if(!parseMathIntoSymbols(symbols, rightMath))
                        return NULL;
                    addSymbolToTable(symbols, lineNum, 0, 'l');
                    return symbols;
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
            if(op && validIfOp(op) ){
                rightMath = strtok(NULL, " ");
                if(rightMath){
                    token = strtok(NULL, " ");  //This is a "goto" token
                    if(token && strcmp(token, "goto") == 0){
                        token = strtok(NULL, " ");  //This is now a line number
                        if(token){
                                //Math is placed into symbol table here
                                //Function returns 1 on success, 0 on failure
                            if(!parseMathIntoSymbols(symbols, leftMath)||!parseMathIntoSymbols(symbols, rightMath))
                                return NULL;

                            lineValuePtr = parseLong(token); //Check for proper input
                            if(lineValuePtr){
                                if((garbage = strtok(NULL, " "))){
                                    printf("Extra symbols (%s) detected after goto statement!\n", garbage);
                                    return NULL;
                                }
                            }
                            else{
                                printf("Non-numeric line number: %s\n", token);
                                return NULL;
                            }

                            addSymbolToTable(symbols, lineNum, 0, 'l');
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
        if((garbage = strtok(NULL, " "))){
            printf("Extra symbols (%s) detected after end statement!\n", garbage);
            return NULL;
        }
        else
            addSymbolToTable(symbols, lineNum, 0, 'l');
        return symbols;
    }
    else{
        printf("Invalid instruction (%s) detected!\n", instruction);
        return NULL;
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

int symbolDoesntExist(StkPtr stk, char *token, char type){
    if(stk == NULL)
        return -1;

    for(int i = 0; i < stk->top; i++){
        if(((SymPtr)stk->stack[i])->type == type)
        if(strcmp(((SymPtr)stk->stack[i])->symbol, token) == 0)
            return 0;
    }
    return 1;

}

int getHighestLineNum(StkPtr stk){
    if(stk == NULL)
        return -1;

    int highest = 0;
    long *test;
    for(int i = 0; i < stk->top; i++){
        if(((SymPtr)stk->stack[i])->type == 'l'){
            if((test = parseLong(((SymPtr)stk->stack[i])->symbol))){
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

int addSymbolToTable(StkPtr symbols, char* symbol, int addr, char type){
    if(symbolDoesntExist(symbols, symbol, type)){
        SymPtr symptr = (SymPtr)malloc(sizeof(Sym));
        symptr->symbol = (char *)malloc(strlen(symbol));
        strcpy(symptr->symbol, symbol);
        symptr->addr = addr;
        symptr->type = type;
        push(symbols, symptr);
        return 1;
    }
    return 0;
}

int parseMathIntoSymbols(StkPtr symbols, char *math){
    StkPtr stkptr = shunt(math);
    char *stackValue;
    if(stkptr){
        if(stackIsValid(stkptr)){
            for(int i = 0; i < stkptr->top; i++){
                stackValue = (char *)stkptr->stack[i];
                if(isdigit(*stackValue)){ //Number
                    addSymbolToTable(symbols, stackValue, getHighestAddress(symbols)+1, 'c');
                }
                else if(precedence(stackValue) == 0){ //Variable
                    addSymbolToTable(symbols, stackValue, getHighestAddress(symbols)+1, 'v');
                }
            }
        }
        else{
            printf("Poorly formatted math: %s\n", math);
            return 0;
        }
    }
    else{
        printf("Math could not be parsed: %s\n", math);
        return 0;
    }
    return 1;
}

int calculateLineNumbers(StkPtr symbols, char *curLine, int offset){
    char *lineNum = strtok(curLine, " ");
    char *instruction = strtok(NULL, " ");
    char *leftMath, *rightMath, *op;

    updateSymbol(symbols, lineNum, 'l', offset);
    if(!strcmp(instruction, "goto")||!strcmp(instruction, "print")||!strcmp(instruction, "input")||!strcmp(instruction, "end")){
        offset++;
    }
    if(!strcmp(instruction, "let")){
        strtok(NULL, " "); //Clear out variable
        strtok(NULL, " "); //Clear out = sign
        rightMath = strtok(NULL, " ");
        offset += 3*countOperators(rightMath); //There are 3 assembly instructions for each arithmetic operator
        offset += 2; //A load and a store operation are necessary at least
    }
    if(!strcmp(instruction, "if")){
        leftMath = strtok(NULL, " ");
        op = strtok(NULL, " ");
        rightMath = strtok(NULL, " ");
        offset += 3*countOperators(leftMath); //There are 3 assembly instructions for each arithmetic operator
        offset += 3*countOperators(rightMath);
        offset += 2; //Plus two for the final difference of the left and right
        offset += 1; //Plus one for the branch instruction
        if(strcmp(op, "<=")==0||strcmp(op, ">=")==0){
            offset+=1; //An extra branch instruction is needed for these
        }
        if(strcmp(op, "!=")==0){
            offset+=3; // x!=y is the same as x>y OR x<y, so an extra three instructions are needed
        }
    }

    return offset;
}

//!strcmp actually means the strings are equal
int validIfOp(char *op){
    return !strcmp(op, "==")||!strcmp(op, ">=")||!strcmp(op, "<=")||!strcmp(op, ">")||!strcmp(op, "<")||!strcmp(op, "!=");
}
