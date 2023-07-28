#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*hash table for macros*/

/*hash function for macro table*/
unsigned long macroHash(int size, char* macroName, unsigned long hash)
{
	for (int i = 0; macroName[i] != '\0'; i++) 
	{
		hash = (hash * 31 + macroName[i]) % size;
	}
	return hash;
}

/*defines the HashTable item*/
typedef struct Ht_item
{
	char* key; /*the macro name*/
	unsigned long hashValue; /*the hash value*/
	char* text; /*the macro content*/
} macroItem;

/*defines the HashTable*/
typedef struct HashTable
{
	macroItem** items;
	int size;
	int count;
	int maxProbs;
} macroTable;

/*create macro item*/
macroItem* createMacro(const char* key, unsigned long value, const char* text)
{
	macroItem* item = (macroItem*)malloc(sizeof(macroItem));
	if (item == NULL)
	{
		printf("Memory allocation failed for new macro. \n");
		exit(1);
	}
	item->key = (char*) malloc(strlen(key) + 1);
	item->hashValue = value;
	item->text = (char*) malloc(strlen(text) + 1);
	strcpy(item->key, key);
	strcpy(item->text, text);
	return item;
}

/*create macro table*/
macroTable* createMacroTable(int size)
{
	macroTable* table = (macroTable*)malloc(sizeof(macroTable));
	table->size = size;
	table->count = 0;
	table->items = (macroItem**)calloc(table->size, sizeof(macroItem*)); /*allocate array*/

	for (int i = 0; i < table->size; i++)
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
	for (int i = 0; i < table->size; i++)
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

	while (item != NULL)
	{
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

/*insert item to the table*/
void insertMacro(macroTable* table, macroItem* newItem, char* key, unsigned long value)
{
	if (!searchMacro(table, key))
	{
		macroItem* ptr = table->items[value];
		int probsCount = 1;

		while (ptr != NULL)
		{
			value =	macroHash(table->size, key, value); /*re-hash*/
			ptr = table->items[value];
			probsCount++;
		}

		newItem->hashValue = value;
		table->items[value] = newItem;
		if (table->maxProbs < probsCount)
			table->maxProbs = probsCount;
	}
}

int main()
{
	unsigned long newHash1 = macroHash(3, "ELAD", 0);
	unsigned long newHash2 = macroHash(3, "HANNAH", 0);
	unsigned long newHash3 = macroHash(3, "PEACHES", 0);
	/*newHash3 = macroHash(3, "PEACHES", 1);
	newHash3 = macroHash(3, "PEACHES", 2);*/
	printf("%lu \n", newHash1);
	printf("%lu \n", newHash2);
	printf("%lu \n", newHash3);
	
	macroTable* table = createMacroTable(3);
	macroItem* ELAD = createMacro ("ELAD", newHash1);
	macroItem* HANNAH = createMacro ("HANNAH", newHash2);
	macroItem* PEACHES = createMacro ("PEACHES", newHash3);
	
	insertMacro(table, ELAD, "ELAD", newHash1);
	if (searchMacro(table, "ELAD"))
		printf("found elad\n");
		
	insertMacro(table, HANNAH, "HANNAH", newHash2);
	printf("inserted hannah\n");
	if (searchMacro(table, "HANNAH"))
		printf("found HANNAH\n");
		
	insertMacro(table, PEACHES, "PEACHES", newHash3);
	printf("inserted peaches\n");
	if (searchMacro(table, "PEACHES"))
		printf("found PEACHES\n");
	
	freeMacroTable(table);
	
	return 1;
}