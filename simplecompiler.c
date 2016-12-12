#include <stdio.h>
#include <string.h>
int translateCode(char *command);
main()
{
	char program[] = "10 input x 20 print x 30 goto 10";
	char output[5] = {0};
	char *tokenPtr;
	tokenPtr = strtok(program, " ");
	while(tokenPtr != NULL)
	{
		if(translateCode(tokenPtr))
		printf("%d", translateCode(tokenPtr));
		puts("");
		tokenPtr = strtok(NULL, " ");
	}
}

int translateCode(char *command)
{
	if(strcmp(command,"input")==0)
	return 10;
	else if(strcmp(command, "print")==0)
	return 11;
	else if(strcmp(command, "goto")==0)
	return 40;
	else
	return 0;
}
