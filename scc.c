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
#define DEBUG 0
#define debug(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

int main(int argv, char *argc[])
{
    if(argv == 2){
        FILE *infile = fopen(argc[1], "r");
        if(infile){
            FILE *outfile = fopen("a.out", "w");
            fseek(infile, 0L, SEEK_END);
            long filesize = ftell(infile);
            char input[filesize];
            int amountRead = 0;
            rewind(infile);
            while(!feof(infile))
            amountRead += fread(input + amountRead, sizeof(char), (int)filesize, infile);

            input[filesize] = 0; //Put null char to avoid garbage at the end!
            debug("%s\n\n********* Parsing Code *********\n", input);

            StkPtr symbolTable = firstPass(input);
            secondPass(input, symbolTable);
            printCode(input, symbolTable, outfile);
            if(symbolTable)
            for(int i = 0; i < symbolTable->top; i++){
                debug("i = %d\n", i);
                debug("Symbol: %s\n", ((SymPtr)symbolTable->stack[i])->symbol);
                debug("Address: %d\n", ((SymPtr)symbolTable->stack[i])->addr);
                debug("Type: %c\n", ((SymPtr)symbolTable->stack[i])->type);
            }
            return 0;
        }
        else{
            printf("Failed to open file!\n");
            return -1;
        }
    }

    if(argv == 4){
        FILE *outfile, *infile;
        if(!strcmp(argc[1], "-o")){
            outfile = fopen(argc[2] , "w");
            infile = fopen(argc[3] , "r");
        }
        else if(!strcmp(argc[2], "-o")){
            infile = fopen(argc[1] , "r");
            outfile = fopen(argc[3] , "w");

        }
        else{
            printf("Yer doin it wrong!\n");
            return -1;
        }
        fseek(infile, 0L, SEEK_END);
        long filesize = ftell(infile);
        char input[filesize];
        int amountRead = 0;
        rewind(infile);
        while(!feof(infile))
        amountRead += fread(input + amountRead, sizeof(char), (int)filesize, infile);

        input[filesize] = 0; //Put null char to avoid garbage at the end!
        debug("%s\n\n********* Parsing Code *********\n", input);

        StkPtr symbolTable = firstPass(input);
        secondPass(input, symbolTable);
        printCode(input, symbolTable, outfile);
        if(symbolTable)
        for(int i = 0; i < symbolTable->top; i++){
            debug("i = %d\n", i);
            debug("Symbol: %s\n", ((SymPtr)symbolTable->stack[i])->symbol);
            debug("Address: %d\n", ((SymPtr)symbolTable->stack[i])->addr);
            debug("Type: %c\n", ((SymPtr)symbolTable->stack[i])->type);
        }
        return 0;
    }
    else{
        printf("Improper number of parameters!\n");
        printf("Proper usage:\n");
        printf("scc sourcefile.sc\n");
        printf("scc sourcefile.sc -o outfile\n");
        return -1;
    }
	return 0;
}
