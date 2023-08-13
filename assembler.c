#include <stdio.h>
#include <string.h>
#include "util.h"
#include "globals.h"
#include "macro_unfold.h"
#include "commands.h"
#include "data_handler.h"
#include "first_pass.h"
#include "second_pass.h"

/* get the file name as an argument, no extension is assumed.*/
int main (int argc, char* argv[])
{
    char* fileName; /* the file name as given as argument */
    bool pre_proccesor, first_pass, second_pass;

	if (argc < 2)
	{
		printf("Usage: %s <filename> <filename> ...\n", argv[0]);
	}
	 /* Processing each file name argument */
    for (int i = 1; i < argc; i++) 
    {
        fileName = argv[i];
        if ((pre_proccesor = macroUnfold(fileName)) == false)
        {
            printf("ERROR: pre proccesor of %s failed.\n", fileName);
            continue;
        }
        if ((first_pass = firstPass(fileName)) == false)
        {
            printf("ERROR: first pass of %s failed.\n", fileName);
            continue;
        }
        if ((first_pass = secondPass(fileName)) == false)
        {
            printf("ERROR: first pass of %s failed.\n", fileName);
            continue;
        }
    }

    return 0;
}