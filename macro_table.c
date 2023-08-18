#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "globals.h"
#include "macro_table.h"

/*macro_table is a hash table for macros management */

/*hash function for macro table*/
unsigned long macroHash(int size, char* macroName, unsigned long hash)
{
	int i;
	for (i = 0; (macroName[i]) != '\0'; i++)
	{
		hash = (hash * 31 + macroName[i]) % size;
	}
	return hash;
}

/*create macro item*/
macroItem* createMacro(const char* key, const char* text)
{
	macroItem* item = (macroItem*)malloc(sizeof(macroItem));
	if (item == NULL)
	{
		printf("Memory allocation failed for new macro. \n");
		exit(0);
	}
	item->key = (char*)malloc(strlen(key) + 1);
	if (item->key == NULL)
	{
		printf("Memory allocation failed for new macro key. \n");
		exit(0);
	}
	item->hashValue = 0;
	item->text = (char*)malloc(strlen(text) + 1);
	if (item->text == NULL)
	{
		printf("Memory allocation failed for new macro text. \n");
		exit(0);
	}
	strcpy(item->key, key);
	strcpy(item->text, text);
	return item;
}

/*create macro table*/
macroTable* createMacroTable(int size)
{
	int i;
	macroTable* table = (macroTable*)malloc(sizeof(macroTable));
	if (table == NULL)
	{
		printf("Memory allocation failed for new macro table. \n");
		exit(0);
	}
	table->size = size;
	table->count = 0;
	table->maxProbs = 1;
	table->items = (macroItem**)malloc(table->size * sizeof(macroItem*));
	if (table->items == NULL)
	{
		printf("Memory allocation failed for new macro table. \n");
		exit(0);
	}

	for (i = 0; i < table->size; i++)
	{
		table->items[i] = NULL;
	}

	return table;
}

/*free macro item*/
void freeMacroItem(macroItem* item)
{
	free(item->key);
	free(item->text);
	free(item);
}

/*free macro table*/
void freeMacroTable(macroTable* table)
{
	int i;
	for (i = 0; i < table->size; i++)
	{
		macroItem* item = table->items[i];

		if (item != NULL)
			freeMacroItem(item);
	}

	free(table->items);
	free(table);
}

/*search macro item in table*/
bool searchMacro(macroTable* table, char* macroName)
{
	unsigned long index = macroHash(table->size, macroName, 0);
	int probsCount = 0; /* to compare to max prob of the table */
	macroItem* item = table->items[index];

	while (item)
	{
		printf("item isnt null\n");/*tester remove*/
		if (strcmp(item->key, macroName) == 0)
			return true;
		if (probsCount == table->maxProbs)
			return false;
		probsCount++;
		index = macroHash(table->size, macroName, index);
		item = table->items[index];
	}
	return false;
}

/*return pointer to existing macro*/
macroItem* getMacro(macroTable* table, char* macroName)
{
	unsigned long index = macroHash(table->size, macroName, 0);
	int probsCount = 0; /* to compare to max prob of the table */
	macroItem* item = table->items[index];

	while (item != NULL)
	{
		if (strcmp(item->key, macroName) == 0)
			return item;
		if (probsCount == table->maxProbs)
			return NULL;
		probsCount++;
		index = macroHash(table->size, macroName, index);
		item = table->items[index];
	}
	return NULL;
}

/*insert item to the table*/
void insertMacro(macroTable* table, macroItem* newItem)
{
	if (!searchMacro(table, newItem->key))
	{
		unsigned long index = macroHash(table->size, newItem->key, newItem->hashValue);
		macroItem* ptr = table->items[index];
		int probsCount = 1; /* counting how many hashes it takes to insert, for more efficient searches later */

		while (ptr != NULL)
		{
			index = macroHash(table->size, newItem->key, index); /*re-hash*/
			ptr = table->items[index];
			probsCount++;
		}

		newItem->hashValue = index;
		table->items[index] = newItem;
		if (table->maxProbs < probsCount)
			table->maxProbs = probsCount;
	}
}