#ifndef ARITHMETIC_H_HEADER
#define ARITHMETIC_H_HEADER
//Associativity of functions is given its own type
typedef enum assoc{left, right} assoc_t;

assoc_t assoc(char *token);
StkPtr shunt(char *math);
char *readToken(char **math);
int stackIsValid(StkPtr stk);
#endif //ARITHMETIC_H_HEADER
