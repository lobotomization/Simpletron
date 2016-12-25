#include "stack.h"
#include "symbols.h"
#include "arithmetic.h"
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
	while(token = readToken(&cursor)){

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
	while(tmp = pop(operators)){
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
