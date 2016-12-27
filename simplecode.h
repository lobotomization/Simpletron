#ifndef SIMPLE_CODE_HEADER
#define SIMPLE_CODE_HEADER
#define READ 10 //Takes user input, stores in ##
#define WRITE 11 //Writes value in ## to console
#define LOAD 20 //Loads the value in ## into the accumulator
#define STORE 21 //Stores the accumulator value in ##
#define ADD 30 //Adds the value in ## to the accumulator
#define SUBTRACT 31 //Subtracts the value in ## from the accumulator
#define DIVIDE 32 //Divides the accumulator by the value in ##
#define MULTIPLY 33 //Multiplies the accumulator by the value in ##
#define POWER 34 //Raises the accumulator to the power of the value in ##
#define BRANCH 40 //Branches to location ##
#define BRANCHNEG 41 //Branches to location ## if accumulator < 0
#define BRANCHZERO 42 //Branches to location ## if accumulator = 0
#define HALT 43 //Ends program execution
#endif
