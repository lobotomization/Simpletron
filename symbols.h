#ifndef SYMBOLS_H_HEADER
#define SYMBOLS_H_HEADER

//Create a type to help work with symbols and symbol tables.
typedef struct symbolentry {
    char *symbol; //The symbol
    char type; //'c' for constant, 'v' for variable, 'l' for line number
    unsigned int addr; //Address the symbol value is stored at
} Sym;
typedef Sym *SymPtr;

StkPtr parseLine(char *line, StkPtr symbols);
StkPtr createSymbolTable(StkPtr st);
StkPtr firstPass(char *inputCode);
StkPtr secondPass(char *inputCode, StkPtr symbols);
void printCode(char *inputCode, StkPtr symbols);
int precedence(char *token);
int getHighestAddress(StkPtr stk);
int variableDoesntExist(StkPtr stk, char *token);
int constantDoesntExist(StkPtr stk, char *token);
int lineDoesntExist(StkPtr stk, char *token);
int highestLineNum(StkPtr stk);
int updateSymbol(StkPtr symbols, char * name, char type, int addr);
int countOperators(char *math);
int getSymbolAddress(StkPtr symbols, char *name, char type);
void mathToSimplecode(StkPtr symbols, char *leftMath, char *op, char *rightMath, char *branch);
int operatorToSimplecode(char a);
#endif //SYMBOLS_H_HEADER
