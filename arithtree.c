#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//A stack of strings is used to help parse the arithmetic.
//Top stores the number of items on the stack
typedef struct stringstack {
	unsigned int top;
    char **stack;
} Stk;
//Give pointers to Stk structs their own type
typedef Stk *StkPtr;
//Associativity of functions is given its own type
typedef enum assoc{left, right} assoc_t;
/*
* newStk - Returns a pointer to a new stack object
* pop - Returns the top-most string from the stack after removing it
* peek - Returns the top-most string from the stack without removing it
* compute - Applies the arithmetic operator specified by the char c
* power - Performs exponentiation of long ints
* parseRPN - This parses and evaluates a stack of arithmetic given in reverse polish notation
* longToString - Converts a long to a string for placement on a stack
* printStack - This prints the strings that currently reside on a stack object
* shunt -
*   This function uses the shunting yard algorithm.
*   This is where standard input is converted
*   into reverse polish notation.
*   It currently handles addition, multiplication
*   subtraction, division and exponentiation of
*   unsigned long ints.
* precedence - This returns the precedence of an arithmetic operator
* assoc - This returns the associativity of an operator (left or right)
* readToken - This parses the user input into numbers and operators
*/
StkPtr newStk();
int push(StkPtr st, char *in);
char *pop(StkPtr st);
char *peek(StkPtr st);
long compute(long a, long b, char c);
long power(long a, long b);
long *parseRPN(StkPtr st);
char *longToString(long l);
void printStack(StkPtr st);
StkPtr shunt(char *math);
int precedence(char *token);
assoc_t assoc(char *token);
char *readToken(char **math);
int main()
{
    /*
	StkPtr test = newStk();
    printf("sizeof(test->stack) = %lu\n", sizeof(test->stack));

	push(test, "ayylmao\n");
	push(test, "lmao\n");
	printf("Test Stack:\n");
	printStack(test);
    printf("sizeof(char*) = %lu\n", sizeof(char*));
    printf("sizeof(test->stack) = %lu\n", sizeof(test->stack));
	printf("Am I an alien?\n%s", pop(test));
	char *math = "(123+456)*(789-42)/98";
	printf("Math: %s\n", math);
    StkPtr out = shunt(math);
    printf("Output stack:\n");
    printStack(out);
    printf("This expression equals %lu\n", parseRPN(out));*/

    char input[1024];
    StkPtr arithStack;
    long *sol;
    printf("Interactive Math Solver\n");
    do{
        printf("M> ");
        scanf("%s", input);
        if(strcmp(input, "quit") == 0)
            break;
        arithStack = shunt(input);
        if(arithStack)
            sol = parseRPN(arithStack);
        if(sol)
            printf("%ld\n", *sol);
    }while(1);
	return 0;
}

long *parseRPN(StkPtr st){
    StkPtr tmp = newStk();
    long a, b, c, *out;
    char *a_st, *b_st;
    for(int i = 0; i < st->top; i++){
        if(isdigit(**(st->stack + i))){
            push(tmp, *(st->stack + i));
        }
        else{
            b_st = pop(tmp);   //Pop the strings backwards since this
            a_st = pop(tmp);  //is how they're stored on the stack
            if(a_st == NULL || b_st == NULL){
                printf("Inappropriate number of parameters!\n");
                return NULL;
            }
            b = strtol(b_st, NULL, 10);
            a = strtol(a_st, NULL, 10);
            c = compute(a, b, **(st->stack + i));
            push(tmp, longToString(c));
        }
    }

    out = malloc(sizeof(long));
    *out = strtol(pop(tmp), NULL, 10);

    return out;

}

char *longToString(long l){
    const int n = snprintf(NULL, 0, "%ld", l);
    char *buf = (char *)malloc(n+1);
    int c = snprintf(buf, n+1, "%ld", l);
    return buf;
}

long compute(long a, long b, char c){
    switch(c){
    case '+':
        return a+b;
    case '-':
        return a-b;
    case '*':
        return a*b;
    case '/':
        return a/b;
    case '^':
        return power(a,b);
    default:
        printf("Bad symbol %c detected during parse!\n", c);
        return 0;
    }
}

long power(long a, long b){
    long out = a;
    b--;
    while(b){
        out *= a;
        b--;
    }
    return out;
}

StkPtr newStk(){
	StkPtr out = (StkPtr)calloc(1, sizeof(Stk));
	out->top = 0;
	return out;
}

int push(StkPtr st, char *in){
	st->stack = (char **)realloc(st->stack, (st->top+1)*sizeof(char*));
	*(st->stack + st->top) = in;
	st->top++;
	return st->top;
}

char *pop(StkPtr st){
	if(st == NULL || st->top == 0)
	return NULL;
	st->top--;
	char *out = (char *)malloc(strlen(*(st->stack + st->top)));
	strcpy(out, *(st->stack + st->top));
	st->stack = (char **)realloc(st->stack, st->top*sizeof(char*));
	return out;
}

char *peek(StkPtr st){
	if(st == NULL || st->top == 0)
	return NULL;
	char *out = (char *)malloc(strlen(*(st->stack + st->top - 1)));
        strcpy(out, *(st->stack + st->top - 1));
        return out;
}

void printStack(StkPtr st){
    if(st == NULL){
        printf("(null)\n");
    }
    else{
        for(int i = 0; i < st->top; i++){
            printf("Stack[%d] = %s\n", i, *(st->stack + i));
        }
    }
}


StkPtr shunt(char *math){
	StkPtr output = newStk(), operators = newStk();
	char *cursor = math, *tmp, *token;
	while(token = readToken(&cursor)){

		if(isdigit(*token)){
			push(output, token);
            continue;
		}

        if(*token == '('){
			push(operators, token);
			continue;
		}

        if(*token == ')'){
            while(tmp = pop(operators)){
                if(*tmp == '(')
                    break;
                push(output, tmp);
            }
            if(tmp == NULL){
                printf("Close bracket without open bracket!\n");
                return NULL;
            }
            continue;
        }

        /*
        *   The following if statement should
        *   only be running if *token != ')'
        *   and *token != '('
        *   This is only being implicitly enforced
        *   due to the above if statements and their
        *   continue clauses.
        */
        //  Precedence of 0 means a bad token was given.
		if(!isdigit(*token) && precedence(token)){

            if(assoc(token) == left)
			while(peek(operators) != NULL && precedence(token) <= precedence(peek(operators))){
				push(output, pop(operators));
			}

            if(assoc(token) == right)
			while(peek(operators) != NULL && precedence(token) < precedence(peek(operators))){
				push(output, pop(operators));
			}

            push(operators, token);
            continue;
		}



		else
		{
			printf("Bad input!\n");
			printf("Token not recognized: %s\n", token);
			return NULL;
		}
	}

	while(tmp = pop(operators)){
		if(*tmp == '(' || *tmp == ')'){
			printf("Mismatched parentheses!\n");
			return NULL;
		}
		push(output, tmp);
	}
	return output;

}
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

assoc_t assoc(char *token){
	if(*token == '^')
	return right;
	return left;
}
char *readToken(char **math){
	if(math == NULL || **math == '\0')
	return NULL;
	char *cursor = (char *)malloc(strlen(*math));
	char *base = cursor;
	if(!isdigit(**math)){
		*cursor = **math;
		cursor++;
		(*math)++;
		*cursor = '\0';
		return base;
	}
	while(isdigit(**math)){
		*cursor = **math;
		cursor++;
		(*math)++;
	}
	*cursor = '\0';
	return base;
}
