#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stack.h"


StkPtr newStk(){
    StkPtr out = (StkPtr)calloc(1, sizeof(Stk));
    out->top = 0;
    return out;
}

int push(StkPtr st, void *in){
    st->stack = (void **)realloc(st->stack, (st->top+1)*sizeof(void*));
    *(st->stack + st->top) = in;
    st->top++;
    return st->top;
}

void *pop(StkPtr st){
    if(st == NULL || st->top == 0)
        return NULL;
    st->top--;
    char *out = (void *)malloc(strlen(*(st->stack + st->top)));
    strcpy(out, *(st->stack + st->top));
    st->stack = (void **)realloc(st->stack, st->top*sizeof(void*));
    return out;
}

void *peek(StkPtr st){
    if(st == NULL || st->top == 0)
        return NULL;
    void *out = (void *)malloc(strlen(*(st->stack + st->top - 1)));
    strcpy(out, *(st->stack + st->top - 1));
    return out;
}

void printStack(StkPtr st){
    if(st == NULL){
        printf("(null)\n");
    }
    else{
        for(int i = 0; i < st->top; i++){
            printf("Stack[%d] = %s\n", i, (char*)*(st->stack + i));
        }
    }
}

