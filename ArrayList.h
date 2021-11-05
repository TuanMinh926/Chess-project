#ifndef ARRAYLIST_H_
#define ARRAYLIST_H_

#include <stdbool.h>
#include <string.h> 
#include <stdlib.h>
#include "ChessGlobalDefinitions.h" 
typedef struct history_element_t {
	BoardSquare oldSquare;
	BoardSquare newSquare;
	char prevElementOnNewCell;
	bool isWhiteKingChecked;
	bool isBlackKingChecked;
} HistoryElement;

typedef struct array_list_t {
	HistoryElement * elements;
	int actualSize;
	int maxSize;
} ArrayList;

typedef enum array_list_message_t {
	ARRAY_LIST_SUCCESS,
	ARRAY_LIST_INVALID_ARGUMENT,
	ARRAY_LIST_FULL,
	ARRAY_LIST_EMPTY
} ARRAY_LIST_MESSAGE;

ArrayList* arrayListCreate(int maxSize);

void arrayListDestroy(ArrayList* src);

ARRAY_LIST_MESSAGE arrayListAddAt(ArrayList* src, HistoryElement elem, int index);

ARRAY_LIST_MESSAGE arrayListAddFirst(ArrayList* src, HistoryElement elem);

ARRAY_LIST_MESSAGE arrayListAddLast(ArrayList* src, HistoryElement elem);

ARRAY_LIST_MESSAGE arrayListRemoveAt(ArrayList* src, int index);

ARRAY_LIST_MESSAGE arrayListRemoveFirst(ArrayList* src);

ARRAY_LIST_MESSAGE arrayListRemoveLast(ArrayList* src);

HistoryElement arrayListGetAt(ArrayList* src, int index);

HistoryElement arrayListGetFirst(ArrayList* src);

HistoryElement arrayListGetLast(ArrayList* src);

int arrayListMaxCapacity(ArrayList* src);

int arrayListSize(ArrayList* src);

bool arrayListIsFull(ArrayList* src);

bool arrayListIsEmpty(ArrayList* src);

#endif
