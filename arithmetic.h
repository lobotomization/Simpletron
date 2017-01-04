#ifndef ARITHMETIC_H_HEADER
#define ARITHMETIC_H_HEADER
#include "stack.h" //Relies on stack functionality
//Associativity of functions is given its own type
typedef enum assoc{left, right} assoc_t;

/*
* max - Returns the max of two integers
* assoc - This returns the associativity of an operator (left or right)
* shunt -
*   This function uses the shunting yard algorithm.
*   This is where standard input is converted
*   into reverse polish notation.
*   It currently handles addition, multiplication
*   subtraction, division and exponentiation of
*   unsigned long ints.
*
* readToken -
*   This function takes the address of a string (i.e. char **) and
*   returns either the longest number it can parse, or the first non-digit
*   character it encounters. The input string will have its pointer moved
*   forward according to how many characters readToken reads, allowing for
*   the same string to be continuously input into readToken until it is read.
*   The output of this function a string (char *) containing the token
*   that readToken read.
*
* countOperators - This returns the number of arithmetic operators in a string of math
* precedence - This returns the precedence of an arithmetic operator
* stackIsValid - Makes sure the number of numbers is one greater than the number of operators
* compute - Applies the arithmetic operator specified by the char c
* parseRPN - This parses and evaluates a stack of arithmetic given in reverse polish notation
* power - Performs exponentiation of long ints, returns a^b
*/
int max(int a, int b);
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
