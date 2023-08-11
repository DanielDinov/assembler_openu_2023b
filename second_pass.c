#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "commands.h"
#include "data_handler.h"
#include "symbol_table.h"
#include "util.h"

/* second_pass gets an .am file and access to symbol table to update and finish the code and data images */

bool secondPass(FILE* file, char* fileName)
{
	char line[MAX_LINE_LEN + 2];
	char* token = NULL;
	int IC = 0;

	while (fgets(line, MAX_LINE_LEN + 2, file) != NULL)
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