#ifndef STACK_H_HEADER
#define STACK_H_HEADER

//A stack of strings is used to help parse the arithmetic.
//Top stores the number of items on the stack
typedef struct voidstack {
unsigned int top;
    void **stack;
} Stk;

//Give pointers to Stk structs their own type
typedef Stk *StkPtr;


/*
* newStk - Returns a pointer to a new stack object
* push - Puts the given pointer on the top of the given stack
* pop - Returns the top-most string from the stack after removing it
* peek - Returns the top-most string from the stack without removing it
* printStack - This prints a stack containing all strings (i.e. char *)
* copyStack - This returns a pointer to a copy a stack
*/

StkPtr newStk();
int push(StkPtr st, void *in);
void *pop(StkPtr st);
void *peek(StkPtr st);
void printStack(StkPtr st);
StkPtr copyStack(StkPtr st);

#endif // STACK_H_HEADER
