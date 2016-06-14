#include "functions.h"
// Functions implementations
/*
change at 14/06/2016 d
NOTES:
1.change the mask in convertFromCompressedBits!
wrong after the second time, numofRooms good, after NOT!
*/

void restoreSize(char * fileName, short int * sizeOfDB, short int * numOfCmds)
{
	FILE *fp = fopen(fileName, "rb");
	short int starter = 0;
	if (fp == NULL) // if its the first run of the program
	{
		fp = fopen(fileName, "wb");
		fwrite(&starter, sizeof(short int), 1, fp);
		fwrite(&starter, sizeof(short int), 1, fp);
		*sizeOfDB = *numOfCmds = starter;
	}
	else
	{
		fread(sizeOfDB, sizeof(short int), 1, fp);
		fread(numOfCmds, sizeof(short int), 1, fp);
	}
	fclose(fp);
}
//readFromBINFILE
void readApDBFromBinFile(char *fileName, short int DBSize, ApList *apDBList)
{
	FILE *input;
	Apartment *newApartment;
	int i;

	makeEmptyApList(apDBList);
	if (DBSize == 0)
		return;

	input = fopen(fileName, "rb");
	checkMemoryAllocation(input);
	for (i = 0; i < DBSize; i++)
	{
		newApartment = getAppFromBinFile(input);
		insertDataToEndApList(apDBList, newApartment);
	}
	*(apDBList->size) = DBSize; //check ?
	fclose(input);
}
Apartment * getAppFromBinFile(FILE *filePtr)
{
	short int adresseLen;
	unsigned int leftPart;
	BYTE rightPart;
	Apartment *newApartment;

	newApartment = (Apartment*)malloc(sizeof(Apartment));
	checkMemoryAllocation(newApartment);

	fread(&newApartment->code, sizeof(short int), 1, filePtr);
	fread(&adresseLen, sizeof(short int), 1, filePtr);
	newApartment->address = (char*)malloc((adresseLen + 1)*sizeof(char));
	fread(newApartment->address, sizeof(char), adresseLen, filePtr);
	newApartment->address[adresseLen] = '\0';
	fread(&newApartment->price, sizeof(int), 1, filePtr);
	//get left and right Parts
	fread(&leftPart, sizeof(unsigned int), 1, filePtr);
	fread(&rightPart, sizeof(BYTE), 1, filePtr);
	convertFromCompressedBits(newApartment, leftPart, rightPart);
	return newApartment;
}

void updateSizeOfDBInFile(char *fileName, short int sizeDB, short int sizeCmd)
{
	FILE *fp;

	fp = fopen(fileName, "wb");
	checkMemoryAllocation(fp);
	fwrite(&sizeDB, sizeof(short int), 1, fp);
	fwrite(&sizeCmd, sizeof(short int), 1, fp);
	fclose(fp);
}
void convertFromCompressedBits(Apartment *newApartment, unsigned int leftPart, BYTE rightPart)
{
	short int leftPartSize = BYTE_SIZE * 4, leftOverBits;
	unsigned int mask1 = ~0, tempMask;

	//left part
	newApartment->numOfRooms = (short int)(leftPart >> (leftPartSize - 4));		//get the first 4 bits

	tempMask = leftPart << 4;													//get rest of numOfRooms(first 4 bits)

	newApartment->enterDate.day = (short int)(tempMask >> (leftPartSize - 5));  //move left partsize - the 5 bits of enterdate.day

	tempMask = leftPart << 9;													//get rest of first 9 bits					
	newApartment->enterDate.month = (short int)(tempMask >> (leftPartSize - 4)); //move leftPart size - 5 bits of month

	tempMask = leftPart << 13;													//get rest of numOfRooms(first 13 bits)				
	newApartment->enterDate.year = (short int)(tempMask >> (leftPartSize - 7)); //move leftPart size - 7 bits of year

	//DB DATE
	tempMask = leftPart << 20;										
	newApartment->DBDate.day = (short int)(tempMask >> (leftPartSize - 5));

	tempMask = leftPart << 25;
	newApartment->DBDate.month = (short int)(tempMask >> (leftPartSize - 4));

	tempMask = leftPart << 29;
	tempMask >>= leftPartSize - 3;
	tempMask <<= 4;
	//right part
	rightPart >>= 4;
	newApartment->DBDate.year = (short int)(rightPart | tempMask);
}
//readFromTXTFILE
void readCmdDBFromTxtFile(CommandList *cmdList, char *short_term_history[], char *fileName, int numOfCmds)
{
	FILE *input;
	int i;
	char temp[MAX_LINE_LEN];

	input = fopen(fileName, "r");
	makeEmptyCommandList(cmdList);

	for (i = 0; i < N; i++)
		short_term_history[i] = NULL;

	if (input == NULL)
		return;


	if (numOfCmds <= N)
		for (i = 0; i < numOfCmds; i++)
		{
			fgets(temp, MAX_LINE_LEN, input);
			short_term_history[i] = (char*)malloc((strlen(temp) + 1) * sizeof(char));
			strcpy(short_term_history[i], temp);
		}
	else //if numofcmd >= 7
	{
		for (i = 0; i < N; i++) //get the first 7 cmds
		{
			fgets(temp, MAX_LINE_LEN, input);
			short_term_history[i] = (char*)malloc((strlen(temp) + 1) * sizeof(char));
			strcpy(short_term_history[i], temp);
		}

		for (i = 0; i < numOfCmds - N; i++)
		{
			fgets(temp, MAX_LINE_LEN, input);
			insertDataToStartCommandList(cmdList, temp);
		}
	}
}

// Commands
void commandFilter(CommandList *cmdList, char *short_term_history[], ApList *apList, char *command)
{
	char dummy, flag, *outCommand;
	int num;

	//1,2,5 cases
	if (command[0] == '!')
	{
		if (command[1] == '!')
			processCommand(apList, short_term_history[0]);
		else
		{
			sscanf(command, "%c%d%c", &dummy, &num, &flag);
			outCommand = getCommandByNumber(num, *cmdList, short_term_history);
			if (flag == '^') //case 5
				outCommand = createModifiedCmd(command, outCommand);
			processCommand(apList, outCommand);
		}
	}
	else if (strcmp(command, "short_history") == 0) //case 3
		printShortHistory(short_term_history, *(cmdList->tail->cmdIndex));
	else if (strcmp(command, "history") == 0)
	{
		printCmdList(*cmdList);
		printShortHistory(short_term_history, *(cmdList->tail->cmdIndex));
	}
	else
		processCommand(apList, command);
}
void updateCommandList(CommandList *cmdList, char *short_term_history[], char *command)
{
	char fileName[12] = "command.txt";
	if (short_term_history[6] != NULL)
		insertDataToEndCommandList(cmdList, short_term_history[6]);
	promoteCommands(short_term_history, command);
	if (short_term_history[0] != NULL)
		short_term_history[0] = (char*)realloc(short_term_history[0], (strlen(command) + 1));
	short_term_history[0] = command;
	updateCommandFile(fileName, command);
	*(cmdList->size) += 1;
}
void updateCommandFile(char *fileName, char *command)
{
	FILE *src = fopen(fileName, "r+");
	FILE *tempFileCopy;
	if (src == NULL)
	{
		src = fopen(fileName, "w");
		checkMemoryAllocation(src);
		fwrite(command, sizeof(char), strlen(command), src);
	}
	else
	{
		tempFileCopy = fopen("tempCopyOfFile", "w+");
		checkMemoryAllocation(tempFileCopy);

		copyFile(src, tempFileCopy);
		fclose(src);
		//create updated file
		src = fopen(fileName, "w");
		//insert last command to the start of command file
		fputs(command, src);
		fputs("\n", src);

		//copy all the rest back to src
		copyFile(tempFileCopy, src);
		fclose(tempFileCopy);

	}
	fclose(src);
}

void copyFile(FILE *src, FILE *dest)
{
	int ch;

	fseek(src, 0, SEEK_SET);
	ch = fgetc(src);
	//make a copy
	while (ch != EOF)
	{
		fputc(ch, dest);
		ch = fgetc(src);
	}
}

void promoteCommands(char *short_term_history[], char *command)
{
	int i;
	for (i = 6; i > 0; i--)
		if (short_term_history[i - 1] != NULL)
			short_term_history[i] = short_term_history[i - 1];

	short_term_history[0] = NULL;
}
int getActualSizeOfArray(char *arr[])
{
	int i = 0;
	while (arr[i] != NULL && i < 7)
		i++;
	return i;
}
char *getCmdByNumInList(int num, CommandList cmdList)
{
	CommandListNode *cur = cmdList.head;
	int i;

	for (i = 0; i < num; i++)
		cur = cur->next;
	return cur->cmdPtr;
}
char *createModifiedCmd(char *input, char *paramaters)
{
	char *token, *sub, *rep;
	const char s[2] = "^"; //maybe need '\^'
	int caret = 1;

	//get str1 and str2
	token = strtok(input, s);
	while (token != NULL)
	{
		if (caret == 2)
		{
			sub = (char*)malloc(strlen(token)*sizeof(char));
			strcpy(sub, token);

		}
		else if (caret == 3)
		{
			rep = (char*)malloc(strlen(token)*sizeof(char));
			strcpy(rep, token);
		}
		token = strtok(NULL, s);
		caret++;
	}
	return str_replace(paramaters, sub, rep);
}
char* str_replace(char* string, const char* substr, const char* replacement)
{
	char* tok = NULL;
	char* newstr = NULL;
	char* oldstr = NULL;
	int   oldstr_len = 0;
	int   substr_len = 0;
	int   replacement_len = 0;

	newstr = strdup(string);
	substr_len = strlen(substr);
	replacement_len = strlen(replacement);

	if (substr == NULL || replacement == NULL) {
		return newstr;
	}

	while ((tok = strstr(newstr, substr))) {
		oldstr = newstr;
		oldstr_len = strlen(oldstr);
		newstr = (char*)malloc(sizeof(char) * (oldstr_len - substr_len + replacement_len + 1));

		if (newstr == NULL) {
			free(oldstr);
			return NULL;
		}

		memcpy(newstr, oldstr, tok - oldstr);
		memcpy(newstr + (tok - oldstr), replacement, replacement_len);
		memcpy(newstr + (tok - oldstr) + replacement_len, tok + substr_len, oldstr_len - substr_len - (tok - oldstr));
		memset(newstr + oldstr_len - substr_len + replacement_len, 0, 1);

		free(oldstr);
	}

	//free(string);

	return newstr;
}
char *getCommandByNumber(int num, CommandList cmdList, char *short_term_history[])
{
	char *output;
	if (isEmptyCommandList(&cmdList))
		output = short_term_history[getActualSizeOfArray(short_term_history) - num]; //
	else
	{
		if (*(cmdList.head->cmdIndex) >= num && num <= *(cmdList.tail->cmdIndex))
			output = getCmdByNumInList(num, cmdList);
		else
			output = short_term_history[7 - (num - *(cmdList.tail->cmdIndex))];
	}
	return output;
}
// Command List functions
CommandListNode * createNewCommandListNode(char *command, CommandListNode * next)
{
	CommandListNode * res = (CommandListNode*)malloc(sizeof(CommandListNode));

	res->cmdPtr = (char*)malloc((strlen(command) + 1)*sizeof(char));
	checkMemoryAllocation(res);
	res->cmdIndex = (short int*)malloc(sizeof(short int));
	checkMemoryAllocation(res->cmdPtr);

	strcpy(res->cmdPtr, command);
	res->next = next;
	return res;
}
void insertDataToEndCommandList(CommandList *lst, char* cmd)
{
	CommandListNode * newTail;

	newTail = createNewCommandListNode(cmd, NULL);
	if (isEmptyCommandList(lst))
		*(newTail->cmdIndex) = 1;
	else
		*(newTail->cmdIndex) = *(lst->tail->cmdIndex) + 1;
	insertNodeToEndCommandList(lst, newTail);

}
void insertNodeToEndCommandList(CommandList *lst, CommandListNode * tail)
{
	if (isEmptyCommandList(lst)) {
		lst->head = lst->tail = tail;
	}
	else {
		lst->tail->next = tail;
		lst->tail = tail;
	}
}
void makeEmptyCommandList(CommandList *lst) {
	lst->head = lst->tail = NULL;
	lst->size = (short int*)malloc(sizeof(short int));
	(*lst->size) = 0;
}
BOOL isEmptyCommandList(CommandList *lst)
{
	if (lst->head == NULL && lst->tail == NULL)
		return TRUE;
	else
		return FALSE;
}
void insertDataToStartCommandList(CommandList *lst, char* command)
{
	CommandListNode * newHead;
	newHead = createNewCommandListNode(command, NULL);
	insertNodeToStartCmdList(lst, newHead);
}
void insertNodeToStartCmdList(CommandList *lst, CommandListNode * head)
{
	if (isEmptyCommandList(lst)) {
		head->next = NULL;
		lst->head = lst->tail = head;
	}
	else {
		head->next = lst->head;
		lst->head = head;
	}
}

// Apartment Lists functions
void makeEmptyApList(ApList *lst) {
	lst->head = lst->tail = NULL;
	lst->size = (short int*)malloc(sizeof(short int));
	*(lst->size) = 0;
}
BOOL isEmptyList(ApList *lst) {
	if (lst->head == NULL && lst->tail == NULL)
		return TRUE;
	else
		return FALSE;
}
ApListNode * getListNode(ApList *lst, int i) {
	ApListNode *curr;
	curr = lst->head;

	while (curr != NULL && i > 0) {
		curr = curr->next;
		i--;
	}
	return curr;
}
ApListNode * createNewApListNode(Apartment* apPtr, ApListNode * next)
{
	ApListNode * res;

	res = (ApListNode *)malloc(sizeof(ApListNode));
	res->apPtr = apPtr;
	res->next = next;
	return res;
}
void insertDataToStartApList(ApList *lst, Apartment* apPtr) {
	ApListNode * newHead;
	newHead = createNewApListNode(apPtr, NULL);
	insertNodeToStartApList(lst, newHead);
}
void insertNodeToStartApList(ApList *lst, ApListNode * head) {
	if (isEmptyList(lst)) {
		head->next = NULL;
		lst->head = lst->tail = head;
	}
	else {
		head->next = lst->head;
		lst->head = head;
	}
}
void insertDataToEndApList(ApList *lst, Apartment* apPtr) {
	ApListNode * newTail;

	newTail = createNewApListNode(apPtr, NULL);
	insertNodeToEndApList(lst, newTail);
}
void insertNodeToEndApList(ApList *lst, ApListNode * tail) {
	if (isEmptyList(lst)) {
		lst->head = lst->tail = tail;
	}
	else {
		lst->tail->next = tail;
		lst->tail = tail;
	}
}
void deleteNodeFromApList(ApList *lst, ApListNode *node, ApListNode *prev)
{

	if (lst->head == lst->tail)
		lst->head = lst->tail = NULL;
	else if (lst->head == node)
		lst->head = lst->head->next;
	else if (lst->tail == node)
	{
		prev->next = NULL;
		lst->tail = prev;
	}
	else
		prev->next = node->next;

	//memmory allocation free
	freeAp(node->apPtr);
	free(node);
}

Apartment** myRealloc(Apartment** apList, int sizeOfDataBase)
{//increase apList by 1
	int i;
	Apartment** newapList;

	newapList = (Apartment**)malloc((sizeOfDataBase + 1)*sizeof(Apartment*));
	for (i = 0; i < sizeOfDataBase; i++)
		newapList[i] = apList[i];
	free(apList);
	return newapList;
}

void checkMemoryAllocation(void *ptr)
{//CHECK IF MEMMORY ALLOCATION SUCCEED
	if (ptr == NULL)
	{
		{
			printf("MEMMORY ALLOCATION FAIL");
			exit(1);
		}
	}
}

// Input
void processCommand(ApList *apList, char *command)
{
	const char s[5] = "-apt";
	char *token;
	char dummyStr[7];
	int code, days;
	token = strstr(command, s) + 5;

	switch (command[0])
	{
	case 'f':
		findByMethod(*apList, token);
		break;
	case 'a':
		addApToDataBase(apList, token);
		break;
	case 'b':
		sscanf(token, "%d", &code);
		buyApartment(apList, code);
		break;
	case 'd':
		sscanf(token, "%s%d", dummyStr, &days);
		deleteApartment(apList, days);
		break;
	default:
		break;
	}
}

// Find Apartment

void findByMethod(ApList apList, char *str)
{
	ApList subList;
	SortingMethods filters;
	int apInd = 0, printType = PRINT_NORNAL;

	//check printType
	printType = checkType(str);

	makeEmptyApList(&subList);
	filters = getFilters(str);
	if (!filters.timeChecker)
	{
		subList = getSubList(apList, filters);
		printByType(subList, printType);
	}
	else
		printDataBaseByTime(apList, filters.timeParameter);
}

SortingMethods getFilters(char *str)
{
	SortingMethods temp;
	int method, parameter;
	const char s[2] = "-";
	char *token, dummyStr[6];

	temp.MinNumRoomsChecker = temp.MaxNumRoomsChecker = temp.MaxPriceChecker = temp.DateChecker = temp.timeChecker = FALSE;

	/* get the first token */
	//#create copy of str (SHOW TO YONI) (30/05/2016)
	char *strCopy = (char*)malloc((strlen(str) + 1)*sizeof(char));
	strcpy(strCopy, str);

	token = strtok(strCopy, s); //changed from str to strCopy becuase its was corrupted
								/* walk through other tokens */
	while (token != NULL)
	{	//check Method
		if (token[0] == 'M' || token[0] == 'D')
		{
			method = findMethodAndParamater(token, &parameter, &temp.tempDate);
			//Sort
			if (method == MAX_PRICE)
			{
				temp.MaxPriceChecker = TRUE;
				temp.maxPriceParameter = parameter;
			}
			else if (method == MAX_NUM_ROOMS)
			{
				temp.MaxNumRoomsChecker = TRUE;
				temp.maxNumRoomsParameter = parameter;
			}
			else if (method == MIN_NUM_ROOMS)
			{
				temp.MinNumRoomsChecker = TRUE;
				temp.minNumRoomsParameter = parameter;
			}
			else if (method == MAX_DATE)
				temp.DateChecker = TRUE;
		}
		else if (token[0] == 'E')
		{
			temp.timeChecker = TRUE;
			sscanf(token, "%s%d", dummyStr, &temp.timeParameter);
		}
		//get new Token
		token = strtok(NULL, s);
	}
	free(strCopy);
	return temp;
}

int findMethodAndParamater(char *subStr, int *paramater, Date *dateParamater)
{//return method
	int i = 0, number = 0;
	int methodRes;
	char token[20], dateNumber[9];

	if (subStr[0] == 'D')
	{
		methodRes = MAX_DATE;
		sscanf(subStr, "%s%s", token, dateNumber);
		strToDate(dateNumber, dateParamater);
	}
	else
	{
		if (strstr(subStr, "MaxP") != NULL)
			methodRes = MAX_PRICE;
		else if (strstr(subStr, "Mi") != NULL)
			methodRes = MIN_NUM_ROOMS;
		else if (strstr(subStr, "MaxN") != NULL)
			methodRes = MAX_NUM_ROOMS;
		sscanf(subStr, "%s%d", token, &number);
		*paramater = number;

	}
	return methodRes;
}

ApList getSubList(ApList apList, SortingMethods filters)
{
	ApList subList;
	ApListNode *curr = apList.head;
	BOOL validAp = TRUE;
	makeEmptyApList(&subList);

	while (curr != NULL)
	{
		if (validAp && filters.DateChecker)
			if (!(checkMinDate(curr->apPtr->enterDate, filters.tempDate)))
				validAp = FALSE;
		if (validAp && filters.MaxNumRoomsChecker)
			if (curr->apPtr->numOfRooms > filters.maxNumRoomsParameter)
				validAp = FALSE;
		if (validAp && filters.MinNumRoomsChecker)
			if (curr->apPtr->numOfRooms < filters.minNumRoomsParameter)
				validAp = FALSE;
		if (validAp && filters.MaxPriceChecker)
			if (curr->apPtr->price > filters.maxPriceParameter)
				validAp = FALSE;

		if (validAp)
			insertDataToEndApList(&subList, curr->apPtr);
		curr = curr->next;

		validAp = TRUE;
	}

	return subList;
}

int checkType(char *str)
{
	int i = 0, length;
	length = strlen(str);
	while (str[i] != '\0')
	{

		if (str[i] == '-')
		{
			if (i + 1 < length && str[i + 1] == 's')
			{
				if (i + 2 < length && str[i + 2] == 'r')
					return PRINT_REVERSED;
				else
					return PRINT_SORTED;
			}
		}
		i++;
	}
	return PRINT_NORNAL;
}

void strToDate(char *token, Date *paramaterDate)
{
	int i = 0;
	paramaterDate->day = paramaterDate->month = paramaterDate->year = 0;

	while (token[i] != '\0')
	{
		if (i < 2)
		{
			paramaterDate->day *= 10;
			paramaterDate->day += token[i] - '0';
		}
		else if (i < 4)
		{
			paramaterDate->month *= 10;
			paramaterDate->month += token[i] - '0';
		}
		else
		{
			paramaterDate->year *= 10;
			paramaterDate->year += token[i] - '0';
		}
		i++;
	}
}

// Add Apartment

void addApToDataBase(ApList *apList, char *str)
{
	char fileName[13] = "apDBFile.bin";

	Apartment * newApartment;

	*(apList->size) += 1;
	newApartment = makeNewAp(*apList, str);
	insertDataToEndApList(apList, newApartment);
	addAppToBinFile(fileName, *newApartment);
}

Apartment * makeNewAp(ApList apList, char *str)
{
	Apartment *newApartment;
	int lengthOfAddress;
	short int newApNumOfRooms, newApDay, newApMonth, newApYear;
	int  newApPrice;

	newApartment = (Apartment*)malloc(sizeof(Apartment));
	checkMemoryAllocation(newApartment);

	// Get the address of the new apartment.
	newApartment->address = getAddressFromString(str, &lengthOfAddress);
	//Get the rest of the details of the new apartment.
	sscanf(str + lengthOfAddress + 1, "%d %hd %hd %hd %hd", &newApPrice, &newApNumOfRooms, &newApDay, &newApMonth, &newApYear);
	newApartment->price = newApPrice;
	newApartment->numOfRooms = newApNumOfRooms;
	newApartment->enterDate.day = newApDay;
	newApartment->enterDate.month = newApMonth;
	newApartment->enterDate.year = newApYear;
	newApartment->code = *(apList.size);
	addEntryTime(&newApartment->DBDate);

	return newApartment;
}

void addEntryTime(EntryToDBDate *DBDate)
{
	time_t rawtime;
	struct tm *info;

	time(&rawtime);
	DBDate->timeInSec = rawtime;
	info = localtime(&rawtime);

	DBDate->day = info->tm_mday;
	DBDate->month = info->tm_mon + 1;
	DBDate->year = info->tm_year + 1900;
}

char * getAddressFromString(char * str, int *lengthOfAddress)
{
	char del = '\"';
	int ind = 0, i;
	int counterDel = 0, sizeOfStr = 0, startOfAddress = 0;
	char* tempAddress;

	while (str[ind] != '\0' &&  counterDel < 2)
	{
		if (str[ind] == del && counterDel == 0)
			startOfAddress = ind + 1;
		if (str[ind] == del)
			counterDel++;
		if (counterDel == 1)
			sizeOfStr++;

		ind++;
	}
	tempAddress = (char*)malloc(sizeOfStr* sizeof(char));
	checkMemoryAllocation(tempAddress);
	ind = 0;
	for (i = startOfAddress; i < sizeOfStr; i++, ind++)
		tempAddress[ind] = str[i];
	tempAddress[sizeOfStr - 1] = '\0';
	*lengthOfAddress = (sizeOfStr);
	return tempAddress;
}

void addAppToBinFile(char *fileName, Apartment  newApartment)
{
	FILE *src;
	short int adresseLen;
	unsigned int leftPart;
	BYTE rightPart;

	src = fopen(fileName, "ab");
	checkMemoryAllocation(src);

	//insert last command to the start of command file
	fwrite(&newApartment.code, sizeof(short int), 1, src);
	adresseLen = (short int)strlen(newApartment.address);
	fwrite(&adresseLen, sizeof(short int), 1, src);
	fwrite(newApartment.address, sizeof(char), adresseLen, src);
	fwrite(&newApartment.price, sizeof(int), 1, src);
	converteToBits(&leftPart, &rightPart, newApartment);
	fwrite(&leftPart, sizeof(unsigned int), 1, src);
	fwrite(&rightPart, sizeof(BYTE), 1, src);
	fclose(src);
}

void converteToBits(unsigned int *leftPart, BYTE *rightPart, Apartment newApartment)
{
	int leftPartSize = BYTE_SIZE * 4;
	unsigned int mask1 = newApartment.numOfRooms;
	unsigned int mask2 = newApartment.enterDate.day;
	unsigned int mask3 = newApartment.enterDate.month;
	unsigned int mask4 = newApartment.enterDate.year - 2000;
	unsigned int mask5 = newApartment.DBDate.day;
	unsigned int mask6 = newApartment.DBDate.month;
	unsigned int mask7 = newApartment.DBDate.year - 2000;
	BYTE mask8 = newApartment.DBDate.year - 2000;

	//left part
	leftPartSize -= 4; // num Of Rooms
	mask1 <<= leftPartSize;
	leftPartSize -= 5; // Entry Day
	mask2 <<= leftPartSize;
	leftPartSize -= 4; // Entry Month
	mask3 <<= leftPartSize;
	leftPartSize -= 7; // Entry Year
	mask4 <<= leftPartSize;
	leftPartSize -= 5; // Entry DB Day
	mask5 <<= leftPartSize;
	leftPartSize -= 4; // Entry DB Month
	mask6 <<= leftPartSize;
	mask7 >>= 4; // Entry DB Year left 3 bits.

	*leftPart = 0;
	*leftPart |= mask1 |= mask2 |= mask3 |= mask4 |= mask5 |= mask6 |= mask7;

	//right part
	mask8 <<= 4;
	*rightPart = mask8;
}

// Buy Apartment
void buyApartment(ApList *apList, int codeNumber)
{
	ApListNode *curr = apList->head;
	ApListNode *prev = curr, *next;
	next = curr->next;
	int nodeIndex = 1;


	while (curr != NULL)
	{
		if (curr->apPtr->code == codeNumber)
		{
			deleteNodeFromApList(apList, curr, prev);
			curr = next;
		}
		if (curr != NULL)
		{
			if (curr->apPtr->code != nodeIndex)
				curr->apPtr->code = nodeIndex;
			prev = curr;
			curr = curr->next;
			if (curr != NULL)
				next = curr->next;
		}
		nodeIndex++;
	}
}
//check of buy ap is working well (check ap code)

//delete
void deleteApartment(ApList *apList, int timeParameter)
{
	ApListNode *curr = apList->head;
	ApListNode *prev = curr;
	ApListNode *next;
	int nodeIndex = 1;


	while (curr != NULL)
	{
		next = curr->next;

		if (checkByTime(curr->apPtr, timeParameter))
		{
			deleteNodeFromApList(apList, curr, prev);
			curr = next;
		}
		if (curr != NULL)
			curr->apPtr->code = nodeIndex++;
		if (curr != next)
			curr = next;
	}
}
void freeAp(Apartment *ap)
{
	free(ap->address);
	free(ap);
}
BOOL checkByTime(Apartment *ap, int daysNumber)
{
	int maxSec;
	maxSec = daysNumber * 24 * 60 * 60;
	time_t curTime, apTime = ap->DBDate.timeInSec;

	time(&curTime);

	if (difftime(curTime, apTime) < maxSec)
		return TRUE;
	else
		return FALSE;
}

//printing
void printByType(ApList apList, int printType)
{
	if (printType == PRINT_REVERSED || printType == PRINT_SORTED)
	{										 // -s or -sr case
		MergeSort(&apList.head);
		if (printType == PRINT_REVERSED)
			printReverse(apList.head);
		else
			printListIter(&apList);
	}
	else									// defualt case
		printListIter(&apList);
}
void printListIter(ApList *lst)
{
	ApListNode *curr = lst->head;

	while (curr != NULL) {
		printAp(curr->apPtr);
		curr = curr->next;
	}
	printf("\n");
}
void printDataBaseByTime(ApList apList, int timeParameter)
{
	ApListNode *curr = apList.head;

	while (curr != NULL)
	{
		if (checkByTime(curr->apPtr, timeParameter));
		printAp(curr->apPtr);
		curr = curr->next;
	}
}
void printReverse(ApListNode* head)
{
	// Base case  
	if (head == NULL)
		return;

	// print the apList after head node
	printReverse(head->next);

	// After everything else is printed, print head
	printAp(head->apPtr);
}
void printAp(Apartment* ap)
{
	printf("code : %d\n", ap->code);
	printf("adress = %s\n", ap->address);
	printf("price = %d\n", ap->price);
	printf("numOfRooms = %d\n", ap->numOfRooms);
	printf("entryDate = %d %d %d\n\n", ap->enterDate.day, ap->enterDate.month, ap->enterDate.year);
}
void printShortHistory(char *short_term_history[], int tailIndex)
{
	int i;
	for (i = 6; i >= 0; i--)
		if (short_term_history[i] != NULL)
			printf("#%d : %s\n", 7 - i + tailIndex, short_term_history[i]);
}
void printCmdList(CommandList cmdList)
{
	CommandListNode *curr = cmdList.head;
	while (curr != NULL)
	{
		printf("#%d : %s\n", *(curr->cmdIndex), curr->cmdPtr);
		curr = curr->next;
	}
}
// Sorting

void MergeSort(ApListNode** headRef)
{
	ApListNode* head = *headRef;
	ApListNode* a;
	ApListNode* b;

	/* Base case -- length 0 or 1 */
	if ((head == NULL) || (head->next == NULL))
		return;

	/* Split head into 'a' and 'b' sublists */
	FrontBackSplit(head, &a, &b);

	/* Recursively sort the sublists */
	MergeSort(&a);
	MergeSort(&b);

	/* answer = merge the two sorted lists together */
	*headRef = SortedMerge(a, b);
}
ApListNode* SortedMerge(ApListNode* a, ApListNode* b)
{
	ApListNode* result = NULL;

	/* Base cases */
	if (a == NULL)
		return(b);
	else if (b == NULL)
		return(a);

	/* Pick either a or b, and recur */
	if (a->apPtr->price <= b->apPtr->price)
	{
		result = a;
		result->next = SortedMerge(a->next, b);
	}
	else
	{
		result = b;
		result->next = SortedMerge(a, b->next);
	}
	return(result);
}
void FrontBackSplit(ApListNode* source, ApListNode** frontRef, ApListNode** backRef)
{
	ApListNode* fast;
	ApListNode* slow;
	if (source == NULL || source->next == NULL)
	{
		/* length < 2 cases */
		*frontRef = source;
		*backRef = NULL;
	}
	else
	{
		slow = source;
		fast = source->next;

		/* Advance 'fast' two nodes, and advance 'slow' one node */
		while (fast != NULL)
		{
			fast = fast->next;
			if (fast != NULL)
			{
				slow = slow->next;
				fast = fast->next;
			}
		}

		/* 'slow' is before the midpoint in the apList, so split it in two
		at that point. */
		*frontRef = source;
		*backRef = slow->next;
		slow->next = NULL;
	}
}
BOOL checkMinDate(Date curr, Date max)
{//return TRUE if curr < max
	BOOL res = FALSE;
	if (curr.year <= max.year)
	{
		if (curr.year < max.year)
			res = TRUE;
		else
		{
			if (curr.month <= max.month)
			{
				if (curr.month < max.month)
					res = TRUE;
				else
				{
					if (curr.day <= max.day)
						res = TRUE;
				}
			}
		}
	}
	return res;
}

