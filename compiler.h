#ifndef COMPILER_H_HEADER
#define COMPILER_H_HEADER
/*
* printCode -
*   This takes a program written in "Simple",
*   along with a completed symbol table (first and second pass completed)
*   and prints out the corresponding Simplecode
*
* mathToSimplecode -
*   This takes a completed symbol table and is used for parsing
*   let and if statements.
*
*   To parse a let statement, the assigned variable
*   is passed into leftMath, the '=' sign (as a string) is passed into op, the expression
*   on the right of the assignment is passed into rightMath, and NULL is passed into branch.
*
*   To parse an if statement the math on the left and right of the comparison operator
*   is passed into leftMath and rightMath, respectively. The comparison operator is passed,
*   as a string, into op. Finally, the line number being branched to is passed into branch.
*
*   operatorToSimplecode - This returns the Simple bytecode corresponding to the given arithmetic operator.
*
*   stackToSimplecode -
*   This function takes the symbol table, the RPN stack of math to convert into code,
*   the temporary working stack (where the final stored address is popped off of!), and
*   the current value of the scratchSpace variable (i.e. the first unused memory address).
*   This function returns the new scratchSpace value.
*
*/
void printCode(char *inputCode, StkPtr symbols, FILE *outfp);
void printCodeLine(StkPtr symbols, char *curLine, FILE *outfp);
void mathToSimplecode(StkPtr symbols, char *leftMath, char *op, char *rightMath, char *branch, FILE *outfp);
int operatorToSimplecode(char a);
int stackToSimplecode(StkPtr symbols, StkPtr stk, StkPtr tmp, int scratchSpace, FILE *outfp);
#endif //COMPILER_H_HEADER
