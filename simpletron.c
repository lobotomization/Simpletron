#include <stdio.h>
#define READ 10 //Takes user input, stores in ##
#define WRITE 11 //Writes value in ## to console
#define LOAD 20 //Loads the value in ## into the accumulator
#define STORE 21 //Stores the accumulator value in ##
#define ADD 30 //Adds the value in ## to the accumulator
#define SUBTRACT 31 //Subtracts the value in ## from the accumulator
#define DIVIDE 32 //Divides the accumulator by the value in ##
#define MULTIPLY 33 //Multiplys the accumulator by the value in ##
#define BRANCH 40 //Branches to location ##
#define BRANCHNEG 41 //Branches to location ## if accumulator < 0
#define BRANCHZERO 42 //Branches to location ## if accumulator = 0
#define HALT 43 //Ends program execution
#define DEBUG 0
#define debug(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

void interpreter(int *memory);
void dump(int *memory, int accumulator, int currentIndex);
main()
{	//{branch to start 00, counter 01, max of counter 02, current max number 03, input 04, counter increment 05,
											              //18                          //24                          //30
//	int memory[100] = {4007,0,7,0,0,1,4300,1002,2002,4206,1004,2004,2103,2001,3005,2101,3102,4230,1004,2004,3103,4124,2004,2103,2001,3005,2101,3102,4230,4018,1103,4300};
//	int memory[100] = {1009, 1010, 2009, 3110, 4107, 1109, 4300, 1110, 4300, 0000, 0000};
//  (1+2)-(24/3)*5 done by me
//	int memory[100] = {4006, 1, 2, 24, 3, 5, 2003, 3204, 3305, 2103, 2001, 3002, 3103, 2101, 1101};
//  (1+2)-(24/3)*5 done by my own mini-compiler (schwwwweeeettt!)
//	int memory[100] = {4006, 1, 2, 24, 3, 5, 2001, 3002, 2101, 2003, 3204, 2103, 2003, 3305, 2103, 2001, 3103, 2101, 1101, 4300};
//	int memory[100] = {0};
    int memory[100] = {4010, 0, 0, 0, 1, 0, 0, 0, 2, 75, 2002, 2101, 2002, 2103, 2001, 3004, 2174, 2074, 2101, 2004, 2105, 2001, 3205, 2174, 2074, 2106, 2006, 3305, 2174, 2001, 3174, 2174, 2074, 2107, 2002, 3107, 4140, 2002, 3107, 4249, 2005, 3004, 2174, 2074, 2105, 2001, 3105, 4163, 4021, 2003, 3004, 2174, 2074, 2103, 2005, 3004, 2174, 2074, 2105, 2001, 3105, 4163, 4021, 2003, 3108, 4170, 2008, 3103, 4170, 1101, 2001, 3109, 4112, 4300, 0};

	printf("***          Welcome to Simpletron!               ***\n");
	printf("***   Please enter your program one instruction   ***\n");
	printf("***   (or data word) at a time. I will type the   ***\n");
    printf("***   location number and a colon. You then type  ***\n");
    printf("*** the word for that location. Type the sentinel ***\n");
    printf("***     -99999 to stop entering your program      ***\n");
	int instruction, memLocation = 0;
	do
	{
		printf("%02d: ", memLocation);
		scanf("%d", &instruction);
		if(instruction == -99999)
		break;
		if(instruction < -9999||instruction > 9999)
		continue;
		memory[memLocation] = instruction;
		memLocation++;
	}
	while(memLocation < 100);
	printf("*** Program loading completed ***\n");
	printf("*** Program execution begins  ***\n");
	interpreter(memory);
	return 0;
}
void interpreter(int *memory)
{
	static int accumulator,currentIndex = 0, fetchedAddress,fetchedInstruction;
	do
	{
		fetchedAddress = memory[currentIndex]%100;
		fetchedInstruction = memory[currentIndex]/100;
		switch(fetchedInstruction)
		{
			case READ:
			printf("Please enter a number into memory location %02d: ",fetchedAddress);
			scanf("%d",&memory[fetchedAddress]);
			currentIndex++;
			break;
			case WRITE:
			printf("The number stored in memory location %02d is: %d\n", fetchedAddress, memory[fetchedAddress]);
			currentIndex++;
			break;
			case LOAD:
			debug("Memory location %02d, containing the number %d, has been stored in the accumulator\n",fetchedAddress,memory[fetchedAddress]);
			accumulator = memory[fetchedAddress];
			currentIndex++;
			break;
			case STORE:
			debug("Memory location %02d now contains %d, the value of the accumulator\n",fetchedAddress,accumulator);
			memory[fetchedAddress] = accumulator;
			currentIndex++;
			break;
			case ADD:
			debug("Memory location %02d, containing the number %d, has been added to the accumulator\n",fetchedAddress,memory[fetchedAddress]);
			accumulator += memory[fetchedAddress];
			if(accumulator>9999)
			{
				printf("Error at memory index %02d\n",currentIndex);
				printf("The accumulator has overflowed, program halted.\n");
				fetchedInstruction = HALT;
				break;
			}
			if(accumulator<-9999)
			{
				printf("Error at memory index %02d\n",currentIndex);
				printf("The accumulator has underflowed, program halted.\n");
				fetchedInstruction = HALT;
				break;
			}
			debug("Current value of the accumulator: %d\n",accumulator);
			currentIndex++;
			break;
			case SUBTRACT:
			debug("Memory location %02d, containing the number %d, has been subtracted from the accumulator\n",fetchedAddress,memory[fetchedAddress]);
			accumulator -= memory[fetchedAddress];
			if(accumulator>9999)
			{
				printf("Error at memory index %02d\n",currentIndex);
				printf("The accumulator has overflowed, program halted.\n");
				fetchedInstruction = HALT;
				break;
			}
			if(accumulator<-9999)
			{
				printf("Error at memory index %02d\n",currentIndex);
				printf("The accumulator has underflowed, program halted.\n");
				fetchedInstruction = HALT;
				break;
			}
			debug("Current value of the accumulator: %d\n",accumulator);
			currentIndex++;
			break;
			case DIVIDE:
			debug("Memory location %02d, containing the number %d, has been divided into the accumulator\n",fetchedAddress,memory[fetchedAddress]);
			if(memory[fetchedAddress]==0)
			{
				printf("Divide by zero error issued by memory index %02d, program halted.", currentIndex);
			}
			accumulator /= memory[fetchedAddress];
			if(accumulator>9999)
			{
				printf("Error at memory index %02d\n",currentIndex);
				printf("The accumulator has overflowed, program halted.\n");
				fetchedInstruction = HALT;
				break;
			}
			if(accumulator<-9999)
			{
				printf("Error at memory index %02d\n",currentIndex);
				printf("The accumulator has underflowed, program halted.\n");
				fetchedInstruction = HALT;
				break;
			}
			debug("Current value of the accumulator: %d\n",accumulator);
			currentIndex++;
			break;
			case MULTIPLY:
			debug("Memory location %02d, containing the number %d, has been multiplied into the accumulator\n",fetchedAddress,memory[fetchedAddress]);
			accumulator *= memory[fetchedAddress];
			if(accumulator>9999)
			{
				printf("Error at memory index %02d\n",currentIndex);
				printf("The accumulator has overflowed, program halted.\n");
				fetchedInstruction = HALT;
				break;
			}
			if(accumulator<-9999)
			{
				printf("Error at memory index %02d\n",currentIndex);
				printf("The accumulator has underflowed, program halted.\n");
				fetchedInstruction = HALT;
				break;
			}
			debug("Current value of the accumulator: %d\n",accumulator);
			currentIndex++;
			break;
			case BRANCH:
			debug("Branching to index %02d\n",fetchedAddress);
			currentIndex = fetchedAddress;
			break;
			case BRANCHNEG:
			debug("Branching to index %02d if the accumulator is negative\n", fetchedAddress);
			debug("Current value of the accumulator: %d\n",accumulator);
			if(accumulator<0)
			currentIndex = fetchedAddress;
			else
			currentIndex++;
			break;
			case BRANCHZERO:
			debug("Branching to index %02d if the accumulator is zero\n", fetchedAddress);
			debug("Current value of the accumulator: %d\n", accumulator);
			if(accumulator==0)
			currentIndex = fetchedAddress;
			else
			currentIndex++;
			break;
			case HALT:
			break;
			default:
			printf("Erroneous instruction %02d issued from memory index %02d, program halted.\n", fetchedInstruction, currentIndex);
			fetchedInstruction = HALT;
			break;
		}
	}
	while(fetchedInstruction != HALT);
	dump(memory, accumulator, currentIndex);
}

void dump(int *memory, int accumulator, int currentIndex)
{
	int units=0,tens=0;
	printf("REGISTERS\n");
	printf("Accumulator\t\t%+05d\n", accumulator);
	printf("Memory Index\t\t   %02d\n", currentIndex);
	printf("Memory Value\t\t%+05d\n", memory[currentIndex]);
	printf("MEMORY\n");
	printf("     ");
	for(units = 0; units<10; units++)
	{
		printf("%4d   ",units);
	}
	puts("");
	for(tens = 0; tens<10; tens++)
	{
		printf("%2d  ", 10*tens);
		for(units = 0; units<10; units++)
		{
			printf("%+05d  ",memory[units + 10*tens]);
		}
		puts("");
	}
}
