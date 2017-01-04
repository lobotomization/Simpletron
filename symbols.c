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
    char *codeCopy = (char *)malloc(strlen(inputCode));
    strcpy(codeCopy, inputCode);
    char *curLine, *nextLine, *lineNum, *instruction;

    int offset = max(getHighestAddress(symbols, 'c'), getHighestAddress(symbols, 'v')) + 1;
    SymPtr symptr;
    for(int i = 0; i < symbols->top; i++){
        symptr = (SymPtr)symbols->stack[i];
        if(symptr->type == 'l')
        updateSymbol(symbols, symptr->symbol, 'l', symptr->addr + offset);
    }

    curLine = strtok_r(codeCopy, "\r\n", &nextLine);
    lineNum = strtok(curLine, " ");
    instruction = strtok(NULL, " ");
    if(strcmp(instruction, "call") == 0){
        char branchConstant[] = "4000";
        char *nextLineAddr = longToString(getNextLineAddress(symbols, lineNum));
        strcpy(branchConstant+4 - strlen(nextLineAddr), nextLineAddr); //This copies the token into the last n zeros (assuming it's n chars long)
        //If an address of length three got into nextLineAddr somehow, bad stuff would happen. Addresses of this length are not supported
        //so they shouldn't be generated or encountered. They probably can be since I have poor bound checking (for example, when using scratchSpace)
        replaceSymbol(symbols, "BRANCH", branchConstant, 'c');
    }
    while((curLine = strtok_r(nextLine, "\r\n", &nextLine))){
        lineNum = strtok(curLine, " ");
        instruction = strtok(NULL, " ");
        if(strcmp(instruction, "call") == 0){
            char branchConstant[] = "4000";
            char *nextLineAddr = longToString(getNextLineAddress(symbols, lineNum));
            strcpy(branchConstant+4 - strlen(nextLineAddr), nextLineAddr); //This copies the token into the last n zeros (assuming it's n chars long)
            replaceSymbol(symbols, "BRANCH", branchConstant, 'c');
        }
    }
    return symbols;
}

StkPtr firstPass(char *inputCode){
    char *codeCopy = (char *)malloc(strlen(inputCode));
    strcpy(codeCopy, inputCode);
    StkPtr symbols = newStk();
    char *curLine, *nextLine;
    int offset = 0;

    curLine = strtok_r(codeCopy, "\r\n", &nextLine);
    if(parseLine(curLine, symbols) == NULL)
        return NULL;

    offset = calculateLineNumbers(symbols, curLine, offset);

    while((curLine = strtok_r(nextLine, "\r\n", &nextLine))){
        if(parseLine(curLine, symbols) == NULL)
            return NULL;

        offset = calculateLineNumbers(symbols, curLine, offset);
    }
    return symbols;
}

StkPtr parseLine(char *instrline, StkPtr symbols){
    char *line = (char *)malloc(strlen(instrline));
    strcpy(line, instrline); //Allow pointer reusability
    char *lineNum, *instruction, *token, *leftMath, *op, *rightMath, *garbage;
    long *lineValuePtr;
    int nextAddress = max(getHighestAddress(symbols, 'c'), getHighestAddress(symbols, 'v')) + 1;

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
    else if(strcmp(instruction, "rem")==0){
        strtok(NULL, "\n"); //Clear out rest of line
        return symbols;
    }
    else if(strcmp(instruction, "input")==0){
        token = strtok(NULL, " ");
        if(token){
            if((garbage = strtok(NULL, " "))){
                printf("Extra symbols (%s) detected on input statement!\n", garbage);
                return NULL;
            }
            else{
                addSymbolToTable(symbols, token, nextAddress, 'v');
                return symbols;
            }
        }
        else{
            printf("Variable missing after input statement!\n");
            return NULL;
        }
    }
    else if(strcmp(instruction, "print")==0){
        token = strtok(NULL, " ");
        if(token){
            if((garbage = strtok(NULL, " "))){
                printf("Extra symbols (%s) detected after print statement!\n", garbage);
                return NULL;
            }
            else{
                addSymbolToTable(symbols, token, nextAddress, 'v');
                return symbols;
            }
        }
        else{
            printf("Variable missing after print statement!\n");
            return NULL;
        }
    }
    else if(strcmp(instruction, "goto")==0){
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
                return symbols;
            }
        }
        else{
            printf("Line number missing after goto statement!\n");
            return NULL;
        }
    }
    else if(strcmp(instruction, "let")==0){
        token = strtok(NULL, " ");
        if(token){
            op = strtok(NULL, " "); //op is = in this case
            if(op && strcmp(op, "=") == 0){
                rightMath = strtok(NULL, " ");
                if(rightMath){
                        //Add variable to symbol table
                    addSymbolToTable(symbols, token, nextAddress, 'v');
                        //Math is placed into symbol table here
                        //Function returns 1 on success, 0 on failure
                    if(!parseMathIntoSymbols(symbols, rightMath))
                        return NULL;
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
    else if(strcmp(instruction, "if")==0){
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
    else if(strcmp(instruction, "end")==0){
        if((garbage = strtok(NULL, " "))){
            printf("Extra symbols (%s) detected after end statement!\n", garbage);
            return NULL;
        }
        return symbols;
    }
    else if(strcmp(instruction, "call")==0){
        token = strtok(NULL, " "); //This is the line number we're on

        if(token){
            if((garbage = strtok(NULL, " "))){
                printf("Extra symbols (%s) detected after call statement!\n", garbage);
                return NULL;
            }
            else{
                //This constant needs to be replaced by a proper branching constant
                // of the form 40XX, where XX points to the address of the next line number

                //The branching constant will be added on the second pass, this was the motivation for the
                //rearrangment of the first and second pass.
                //Since the line numbers are now completely deterministic and simple to compute, I
                //can compute the "next line address" which will be returned to.

                //This placeholder is added so that it can be updated in the second pass
                //as well as to give the line numbers the proper offset.
                //This is a pretty bad kludge tbh, not sure what a better option is at this point.
                char branchName[] = "BRANCH";
                int brID = 0;
                strcpy(branchName+6, longToString(brID));
                while(!addSymbolToTable(symbols, branchName, nextAddress, 'c')){
                    brID++;
                    strcpy(branchName+6, longToString(brID));
                }
            }
        }
        else{
            printf("Line number missing after call statement!\n");
            return NULL;
        }
        return symbols;
    }
    else if(strcmp(instruction, "return")==0){
        if((garbage = strtok(NULL, " "))){
            printf("Extra symbols (%s) detected after end statement!\n", garbage);
            return NULL;
        }
        return symbols;
    }
    else{
        printf("Invalid instruction (%s) detected!\n", instruction);
        return NULL;
    }
    printf("This shouldn't happen!\n");
    return NULL;
}

//These are meant to be used on a StkPtr full of SymPtr's
int getHighestAddress(StkPtr stk, char type){
    if(stk == NULL)
        return -1;

    int out = 0;
    for(int i = 0; i < stk->top; i++){
        if(((SymPtr)stk->stack[i])->type == type)
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
    int nextAddress;
    if(stkptr){
        if(stackIsValid(stkptr)){
            for(int i = 0; i < stkptr->top; i++){
                stackValue = (char *)stkptr->stack[i];
                nextAddress = max(getHighestAddress(symbols, 'c'), getHighestAddress(symbols, 'v')) + 1;
                if(isdigit(*stackValue)){ //Number
                    addSymbolToTable(symbols, stackValue, nextAddress, 'c');
                }
                else if(precedence(stackValue) == 0){ //Variable
                    addSymbolToTable(symbols, stackValue, nextAddress, 'v');
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
    char *lineCopy = (char *)malloc(strlen(curLine));
    strcpy(lineCopy, curLine);
    char *lineNum = strtok(lineCopy, " ");
    char *instruction = strtok(NULL, " ");
    char *leftMath, *rightMath, *op;

    addSymbolToTable(symbols, lineNum, offset, 'l');
    if(!strcmp(instruction, "goto")||!strcmp(instruction, "print")||!strcmp(instruction, "input")||!strcmp(instruction, "end")||!strcmp(instruction, "return")){
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
    if(!strcmp(instruction, "call"))
    offset+=3; //Call requires a load/store/branch

    return offset;
}

//!strcmp actually means the strings are equal
int validIfOp(char *op){
    return !strcmp(op, "==")||!strcmp(op, ">=")||!strcmp(op, "<=")||!strcmp(op, ">")||!strcmp(op, "<")||!strcmp(op, "!=");
}

int getNextLineAddress(StkPtr symbols, char *curLineNum){
    SymPtr symptr;
    int havePassedLine = 0;
    for(int i = 0; i < symbols->top; i++){
            symptr = (SymPtr)symbols->stack[i];
        if(!strcmp(symptr->symbol, curLineNum)){
            havePassedLine = 1;
            continue;
        }
        if(havePassedLine && symptr->type == 'l')
            return getSymbolAddress(symbols, symptr->symbol, 'l');
    }
    return 0;
}


int replaceSymbol(StkPtr symbols, char *oldSym, char *newSym, char type){
    SymPtr symptr;
    int oldSymLen = strlen(oldSym);
    for(int i = 0; i < symbols->top; i++){
        symptr = (SymPtr)symbols->stack[i];
        if(symptr->type == type)
            if(strncmp(symptr->symbol, oldSym, oldSymLen) == 0){
                strcpy(symptr->symbol, newSym);
                return 1;
            }
    }
    return 0;
}
