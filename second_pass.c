#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "util.h"
#include "symbol_table.h"

/* second_pass gets an .am file and access to symbol table to update and finish the code and data images */

FILE* secondPass(FILE* file, char* fileName)
{
	char line[ROW_MAX_LENGTH];
	char* token = NULL;
	int IC = 0;

	while (fgets(line, ROW_MAX_LENGTH, file) != NULL)
	{
		token = strtok(line, delims);
		while (token != NULL)
		{
			if (token[strlen(token) - 1] == ':')
				continue;

			if (does_symbol_exist(find_symbol(token)))
			{
				token = strtok(NULL, delims);
			}

			if (isDirective(token) != 0)
			{

			}
		}
	}
}
