#ifndef SYMBOLS_H_HEADER
#define SYMBOLS_H_HEADER

//Create a type to help work with symbols and symbol tables.
typedef struct symbolentry {
    char *symbol; //The symbol
    char type; //'c' for constant, 'v' for variable, 'l' for line number
    unsigned int addr; //Address the symbol value is stored at
} Sym;
//Create a type for pointers to symbols
typedef Sym *SymPtr;

/*
*   This compiler uses a crude 'two-pass' symbol table system.
*   The first pass collects all of the line numbers, constants and variables.
*   As constants and variables are collected they are given
*   sequentially increasing addresses, starting from 1.
*   Address 0 is always reserved for a branch instruction, which is
*   used to jump past the symbol table at the top of a program.
*   Line numbers wil not be given addresses on the first pass.
*   The second pass will merely give the line numbers sequentially
*   increasing addresses, starting directly after the last
*   address given to a constant or variable.
*/


/*
* parseLine -
*   This function parses the Simple language one line at a time.
*
*   Large amounts of syntactical error checking is done in this function,
*   which increases its complexity.
*
* firstPass -
*   Loads symbols, provides addresses to variables and constants.
*   Line numbers are given relative addresses here, correct up to an offset.
*   Branching constants (for call/return pairs) are given placeholders here.
*   Takes a full Simple program as input and produces a proto-symbol table.
*
* secondPass -
*   The addresses of line numbers are shifted by an offset calculated here.
*   This function takes the same Simple program as the first pass, as well as the
*   proto-symbol table produced by the first pass. The proto-symbol table specified
*   by the symbols variable will have its line numbers updated. Branching constants are replaced by their appropriate values.

The output of this
*   function will be the symbol table. Since the symbol table will already
*   be changed this output is redundant.
*
* symbolDoesntExist - Checks for the existence of a symbol
* updateSymbol - Updates the address of the given symbol
* getHighestAddress - Gets the highest address of a given symbol type currently assigned in a symbol table
* getHighestLineNum - Gets the highest line number in a symbol table
* getSymbolAddress - Gets the address assigned to a symbol from a symbol table
* addSymbolToTable - Adds the symbol to the symbol table if it doesn't already exist.
* parseMathIntoSymbols - This parses in-fix arithmetic into symbols and places them into the symbol table.
* calculateLineNumbers -
*   This function is used in secondPass to calculate the actual line numbers.
*   calculateLineNumbers is extremely sensitive to how let and if statements are parsed!
*
*   Knowing the exact length of the bytecode produced by let and if statements
*   is required for this function to do its job. Therefore, if any changes are made
*   in how the compiler handles these functions which changes the length of the
*   corresponding bytecode, these changes must be reflected in this function.
*
* validIfOp - This function makes sure an operator in an if statement is valid
* getNextLineAddress - Gets the memory address of the line after the given line number.
* replaceSymbol -
*   If oldSym matches the initial chars of a symbol, the *first such* matching symbol is replaced with newSym.
*   Very handy for replacing branch placeholder symbols like "BRANCH0", "BRANCH1", etc. with appropriate branching constants

*/

StkPtr parseLine(char *line, StkPtr symbols);
StkPtr firstPass(char *inputCode);
StkPtr secondPass(char *inputCode, StkPtr symbols);
int symbolDoesntExist(StkPtr stk, char *token, char type);
int updateSymbol(StkPtr symbols, char * name, char type, int addr);
int getHighestAddress(StkPtr stk, char type);
int getHighestLineNum(StkPtr stk);
int getSymbolAddress(StkPtr symbols, char *name, char type);
int addSymbolToTable(StkPtr symbols, char* symbol, int addr, char type);
int parseMathIntoSymbols(StkPtr symbols, char *math);
int calculateLineNumbers(StkPtr symbols, char *curLine, int offset);
int validIfOp(char *op);
int getNextLineAddress(StkPtr symbols, char *curLineNum);
int replaceSymbol(StkPtr symbols, char *oldSym, char *newSym, char type);
#endif //SYMBOLS_H_HEADER
