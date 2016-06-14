#ifndef __functions_h
#define __functions_h

//change at 14/06/2016


#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define N 7
#define BYTE_SIZE 8
#define MAX_LINE_LEN 256
#define INIT_SIZE 1
#define MAX_PRICE 1
#define MAX_NUM_ROOMS 2
#define MIN_NUM_ROOMS 3
#define MAX_DATE 4
#define PRINT_REVERSED 11 
#define PRINT_SORTED 22
#define	PRINT_NORNAL  33

typedef int BOOL;
#define FALSE 0
#define TRUE 1

typedef unsigned char BYTE;

typedef struct date {
	short int day;
	short int month;
	short int year;
}Date;

typedef struct entryToDBDate {
	short int day;
	short int month;
	short int year;
	time_t timeInSec;
}EntryToDBDate;

typedef struct apartment {
	short int code;
	char *address;
	int price;
	short int numOfRooms;
	Date enterDate;
	EntryToDBDate DBDate;
}Apartment;

typedef struct apListNode {
	Apartment* apPtr;
	struct apListNode * next;
} ApListNode;

typedef struct apList {
	ApListNode * head;
	ApListNode * tail;
	short int *size;
}ApList;

typedef struct  commandListNode {
	char* cmdPtr;
	struct commandListNode * next;
	short int *cmdIndex;
} CommandListNode;

typedef struct commandList {
	CommandListNode * head;
	CommandListNode * tail;
	short int *size;
}CommandList;

typedef struct sortingMethods
{
	BOOL MinNumRoomsChecker, MaxNumRoomsChecker, MaxPriceChecker, DateChecker, timeChecker;
	int minNumRoomsParameter, maxNumRoomsParameter, maxPriceParameter, timeParameter;
	Date tempDate;
}SortingMethods;

void restoreSize(char * fileName, short int * sizeOfDB, short int * numOfCmds);
//readFromBinFile
void readApDBFromBinFile(char *fileName, short int DBSize, ApList *apDBList);
void updateSizeOfDBInFile(char *fileName, short int sizeDB, short int sizeCmd);
Apartment * getAppFromBinFile(FILE *filePtr);
void convertFromCompressedBits(Apartment *newApartment, unsigned int leftPart, BYTE rightPart);
//readFromTXTFILE
void readCmdDBFromTxtFile(CommandList *cmdList, char *short_term_history[], char *fileName, int numOfCmds);

//commands
void commandFilter(CommandList *cmdList, char *short_term_history[], ApList *apList, char *command);
void updateCommandList(CommandList *commandList, char *short_term_history[], char *command);
void updateCommandFile(char *fileName, char *command);
void copyFile(FILE *src, FILE *dest);
void promoteCommands(char *short_term_history[], char *command);
int getActualSizeOfArray(char *arr[]);
char *getCommandByNumber(int num, CommandList cmdList, char *short_term_history[]);
char *getCmdByNumInList(int num, CommandList cmdList);
char *createModifiedCmd(char *input, char *paramaters);
char* str_replace(char* string, const char* substr, const char* replacement);
// Command List functions
void makeEmptyCommandList(CommandList *lst);;
CommandListNode * createNewCommandListNode(char *command, CommandListNode * next);
void insertDataToEndCommandList(CommandList *lst, char* cmd);
void insertNodeToEndCommandList(CommandList *lst, CommandListNode * tail);
BOOL isEmptyCommandList(CommandList *lst);
void insertDataToStartCommandList(CommandList *lst, char* command);
void insertNodeToStartCmdList(CommandList *lst, CommandListNode * head);
// ApList functions.
void printListIter(ApList *lst);
void makeEmptyApList(ApList *lst);
BOOL isEmptyList(ApList *lst);
ApListNode* createNewApListNode(Apartment* apPtr, ApListNode * next);
void insertNodeToEndApList(ApList *lst, ApListNode * tail);
void deleteNodeFromApList(ApList *lst, ApListNode *node, ApListNode *prev);
void insertNodeToStartApList(ApList *lst, ApListNode * head);
void insertDataToEndApList(ApList *lst, Apartment* apPtr);
// Functions Declerations:

Apartment** myRealloc(Apartment** apList, int sizeOfDataBase);
void checkMemoryAllocation(void *ptr);
void processCommand(ApList *apList, char *command);

// Find
void findByMethod(ApList apList, char *str);
SortingMethods getFilters(char *str);
int findMethodAndParamater(char *subStr, int *paramater, Date *dateParamater);
ApList getSubList(ApList apList, SortingMethods filters);
int checkType(char *str);


// Add Apartment
void addApToDataBase(ApList *apList, char *str);
Apartment * makeNewAp(ApList apList, char *str);
char * getAddressFromString(char * str, int * lengthOfAddress);
void addAppToBinFile(char *fileName, Apartment  newApartment);
void addEntryTime(EntryToDBDate *DBDate);
void converteToBits(unsigned int *leftPart, BYTE *rightPart, Apartment newApartment);
// Buy Apartment
void buyApartment(ApList *apList, int codeNumber);

// delete Apartment
void deleteApartment(ApList *apList, int timeParameter);
BOOL checkByTime(Apartment *ap, int daysNumber);
void freeAp(Apartment *ap);
// Convertions
int strToInt(char *str);
void strToDate(char *token, Date *paramaterDate);

// Printing
void printByType(ApList apList, int printType);
void printListIter(ApList * lst);
void printReverse(ApListNode* head);
void printAp(Apartment* ap);
void printapListReversed(Apartment **apList, int sizeOfapList);
void printDataBaseByTime(ApList apList, int timeParameter);
void printShortHistory(char *short_term_history[], int tailIndex);
void printCmdList(CommandList cmdList);
// Sotring
void MergeSort(ApListNode** headRef);
ApListNode* SortedMerge(ApListNode* a, ApListNode* b);
void FrontBackSplit(ApListNode* source, ApListNode** frontRef, ApListNode** backRef);
Apartment** createSubDataBaseByTime(Apartment **apList, int *sizeOfDataBase, int days);
BOOL checkMinDate(Date curr, Date max);

#endif