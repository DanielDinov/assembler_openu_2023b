#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "globals.h"
#include "macro_unfold.h"
#include "commands.h"
#include "data_handler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "symbol_table.h"

/* get the file name as an argument, no extension is assumed.*/
int main (int argc, char* argv[])
{
    int i;
    char* fileName; /* the file name as given as argument */
    bool pre_proccesor, first_pass, second_pass;

	if (argc < 2)
	{
		printf("Usage: %s <filename> <filename> ...\n", argv[0]);
	}
	 /* Processing each file name argument */
    
    for (i = 1; i < argc; i++) 
    {   
        create_new_symbol_list();
        fileName = argv[i];
        if ((pre_proccesor = preprocessor(fileName)) == false)
        {
            printf("ERROR: pre preprocessor of %s failed.\n", fileName);
            free_list();
            continue;
        }
        
        if ((first_pass = firstPass(fileName)) == false)
        {
            printf("ERROR: first pass of %s failed.\n", fileName);
            free_list();
            continue;
        }
        printf("first pass finished\n");
        if ((second_pass = secondPass(fileName)) == false)
        {
            printf("ERROR: second pass of %s failed.\n", fileName);
            free_list();
            continue;
        }
        free_list();
        printf("second pass finished\n");
    }

    return 0;
}
