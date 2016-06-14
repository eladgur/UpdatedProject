#include "functions.h"
//change at 14/6/2016

/*
-ELAD:read from apdata base bin file fixed.
-need to change the save of !<> command by the correct command
*/

///////////////////////
void main()
{
	//Declerations
	ApList apDBList;
	CommandList cmdList;
	char fileName[13] = "apDBFile.bin";
	char sizeOfDB[13] = "sizeOfDB.bin";
	char nameOfTxtFile[12] = "command.txt";
	char *short_term_history[N], *input;
	int i = 0;
	short int DBSize, cmdSize;
	restoreSize(sizeOfDB, &DBSize, &cmdSize);
	readCmdDBFromTxtFile(&cmdList, short_term_history, nameOfTxtFile, cmdSize);
	readApDBFromBinFile(fileName, DBSize, &apDBList);

	

	printf("Please enter one of the following commands:\n");
	printf("add-apt, find - apt, buy - apt or delete - apt\n");
	printf("For reconstruction commands, please enter :\n");
	printf("!!, !num, history, short_history or !^str1^str2\n");

	input = (char*)malloc(MAX_LINE_LEN*sizeof(char));
	gets(input);

	while (strcmp(input, "exit") != 0)
	{
		commandFilter(&cmdList, short_term_history, &apDBList, input);
		updateCommandList(&cmdList, short_term_history, input);
		input = (char*)malloc(MAX_LINE_LEN*sizeof(char));
		gets(input);
	}
	updateSizeOfDBInFile(sizeOfDB, *apDBList.size, *cmdList.size);
	printf("\nbye bye\n\n");
}
