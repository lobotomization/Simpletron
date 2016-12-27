#include "stack.h"
#include "symbols.h"
#include "arithmetic.h"
#include "longs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


assoc_t assoc(char *token){
	if(*token == '^')
	return right;
	return left;
}

StkPtr shunt(char *math){
	StkPtr output = newStk(), operators = newStk();
	char *cursor = math, *tmp, *token;
	while((token = readToken(&cursor))){

        if(*token == '('){
			push(operators, token);
			continue;
		}

        if(*token == ')'){
            while((tmp = pop(operators))){
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
        //  Precedence of 0 means a non-operator token was given.
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

        else{
            push(output, token);
			continue;
        }
	}
	while((tmp = pop(operators))){
		if(*tmp == '(' || *tmp == ')'){
			printf("Mismatched parentheses!\n");
			return NULL;
		}
		push(output, tmp);
	}
	return output;

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

int countOperators(char *math){
    int count = 0;
    while(*math != '\0'){
        if(precedence(math))
            count++;
        math++;
    }
    return count;
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

int stackIsValid(StkPtr stk){
    if(stk == NULL)
        return 0;

    int numCount = 0, opCount = 0;
    for(int i = 0; i < stk->top; i++){
        if(precedence((char *)stk->stack[i])){
            opCount++;
        }
        else{
            numCount++;
        }
        if(numCount - opCount < 1)
            return 0;
    }
    if(numCount - opCount != 1)
        return 0;
    else
        return 1;
}

long *parseRPN(StkPtr st){
    StkPtr tmp = newStk();
    long a, b, c, *out;
    char *a_st, *b_st, *cur;
    for(int i = 0; i < st->top; i++){
        cur = (char *)*(st->stack + i);
        if(isdigit(*cur)){
            push(tmp, cur);
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
            c = compute(a, b, *cur);
            push(tmp, longToString(c));
        }
    }

    out = malloc(sizeof(long));
    *out = strtol(pop(tmp), NULL, 10);

    return out;

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

