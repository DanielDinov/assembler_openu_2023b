#include "util.h"
#include "globals.h"
#include "preprocessor.h"
#include "commands.h"
#include "data_handler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "prints.h"

/* assembler gets file names through terminal and proccess them one at a time, assuming file name given with no extension */
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
            removeOutputs(fileName);
            continue;
        }
        if ((first_pass = firstPass(fileName)) == false)
        {
            printf("ERROR: first pass of %s failed.\n", fileName);
            free_list();
            removeOutputs(fileName);
            continue;
        }
        if ((second_pass = secondPass(fileName)) == false)
        {
            printf("ERROR: second pass of %s failed.\n", fileName);
            free_list();
            removeOutputs(fileName);
            continue;
        }
        free_list();

        printOBJ(fileName);
    }

    return 0;
}
