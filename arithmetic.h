#ifndef ARITHMETIC_H_HEADER
#define ARITHMETIC_H_HEADER
#include "stack.h" //Relies on stack functionality
//Associativity of functions is given its own type
typedef enum assoc{left, right} assoc_t;

/*
* assoc - This returns the associativity of an operator (left or right)
* shunt -
*   This function uses the shunting yard algorithm.
*   This is where standard input is converted
*   into reverse polish notation.
*   It currently handles addition, multiplication
*   subtraction, division and exponentiation of
*   unsigned long ints.
*
* precedence - This returns the precedence of an arithmetic operator
* compute - Applies the arithmetic operator specified by the char c
* parseRPN - This parses and evaluates a stack of arithmetic given in reverse polish notation
* power - Performs exponentiation of long ints, returns a^b
*/

assoc_t assoc(char *token);
StkPtr shunt(char *math);
char *readToken(char **math);
int countOperators(char *math);
int precedence(char *token);
int stackIsValid(StkPtr stk);
long compute(long a, long b, char c);
long *parseRPN(StkPtr st);
long power(long a, long b);
#endif //ARITHMETIC_H_HEADER
