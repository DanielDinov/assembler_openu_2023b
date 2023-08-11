#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"
#include "macro_unfold.h"

/* beta version for preAsmblr */
/* get the file name as an argument, no extension is assumed.*/
int main (int argc, char* argv[])
{
    char* fileName; /* the file name as given as argument */
    char* fileNameToOpen; /* modified with .as */

	if (argc < 2)
	{
		printf("Usage: %s <filename> <filename> ...\n", argv[0]);
	}
	 /* Processing each file name argument */
    for (int i = 1; i < argc; i++) 
    {
        fileName = argv[i];
        fileNameToOpen = argv[i];
        strcat(fileNameToOpen, as_extension);
        FILE* file = fopen(fileNameToOpen, "r+");

        if (file == NULL) 
        {
            printf("Failed to open file: %s\n", fileName);
            continue; /* Move on to the next file */
        }

        FILE* amOutputFile = macroUnfold(file, fileName); /* unfold macros, return new file with .am extension */
        /* call first and second read on .am file */
        fclose(file);
    }

    return 0;
	
}
