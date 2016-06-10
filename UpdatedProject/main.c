#include "functions.h"
//change at 10/6/2016

/*
להמשיך את את הפונקציה  readcmddbfromtxtfile

לבדוק שפוינטרים של הגדלים תקינים

לבדוק את כל הקבצים גג

לבדוק קריאה מקובץ וכתיבה לקובץ בינארי וטקסט
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
	restoreSize(sizeOfDB, DBSize,cmdSize);
	readCmdDBFromTxtFile(&cmdList,short_term_history, nameOfTxtFile,cmdSize);
	readApDBFromBinFile(fileName, DBSize, &apDBList);

	input = (char*)malloc(MAX_LINE_LEN*sizeof(char));
	gets(input);



	while (strcmp(input, "exit") != 0)
	{
		commandFilter(&cmdList, short_term_history, &apDBList, input);
		updateCommandList(&cmdList, short_term_history, input);
		input = (char*)malloc(MAX_LINE_LEN*sizeof(char));
		gets(input);

	}
	updateSizeOfDBInFile(sizeOfDB, *apDBList.size,*cmdList.size);
	printf("\nbye bye\n\n");
}
