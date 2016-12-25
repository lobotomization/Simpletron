#ifndef STACK_H_HEADER
#define STACK_H_HEADER

//A stack of strings is used to help parse the arithmetic.
//Top stores the number of items on the stack
typedef struct stringstack {
unsigned int top;
    void **stack;
} Stk;

//Give pointers to Stk structs their own type
typedef Stk *StkPtr;

StkPtr newStk();
int push(StkPtr st, void *in);
void *pop(StkPtr st);
void *peek(StkPtr st);
void printStack(StkPtr st);

#endif // STACK_H_HEADER
