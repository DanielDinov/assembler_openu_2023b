#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "util.h"
#include "macro_table.h"
#define ROW_MAX_LENGTH 81
#define WORD_MAX_LENGTH 80
/*TODO: add define with words like mcro,endmcro etc...*/

/* first read: counting amount of macros and create a suitble hash table
 * second read: store all the macros and their content
 * third read: replace macro tags with the content
 */

FILE* mcroUnfold(FILE* file)
{
    const char delims[4] = " \n\t"; /* TODO: figure out what should be ditched out of every line */
    bool openMacro = false; /* will switch to true when mcro identified, return to false when encounter endmcro */
    int counter = 0; /* how many macros will be in the hash table*/
    int macroLength = 0; /* count how many char to allocate for macros */
    unsigned long hash = 0;
    char* token = NULL;
    filePtr = fseek(file, 0, SEEK_SET);

    /* (1) below insert function to go through file and send it to read_row, get rid of comments and empty lines - the end of this function we will have the new 'am Output' file*/
    /* can use read row provide, or just create ur own version based on it*/


    char line[ROW_MAX_LENGTH]; /* TODO: add a checker for max 80 excluding \n */
    char word[WORD_MAX_LENGTH];

    /* (2) the loop below counts the amount of macros in the file to initialize and efficient table */
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strlen(line) > ROW_MAX_LENGTH)
        {
            /* add error to errors file TODO */
                continue;
        }

        token = strtok(line, delims);
        while (token != NULL)
        {
            if (strcmp(token, "endmcro") == 0)
            {
                openMacro = false;
            }

            if (openMacro)
            {
                macroLength = macroLength + strlen(token) + 1; /*1 for space*/
            }

            if (strcmp(token, "mcro") == 0)
            {
                openMacro = true;
                counter++;
            }
            token = strtok(NULL, delims);
        }
    }
    rewind(file);

    /* initialize new table and a suitble string for macros contnet*/
    macroTable* MACROS = createMacroTable(counter);
    char* macroContent = (char*)malloc(macroLength * sizeof(char)); /* initializng a char array large enough to fit macro content */
    if (macroContent == NULL)
    {
        printf("memory allocation failed.");
        exit(1);
    }

    /* (3) the loop below read the file and insert new macros to the hash table */
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strlen(line) > ROW_MAX_LENGTH)
        {
            /* TODO: add error to errors file */
            continue;
        }

        token = strtok(line, delims);
        while (token != NULL)
        {
            /* endmcro: submit the contnet in and reset the string*/
            if (strcmp(token, "endmcro") == 0)
            {
                newMacro->text = strdup(macroContent);
                openMacro = false;
                macroContent[0] = '\0';
            }

            /* openMacro: add new words to content buffer*/
            if (openMacro)
            {
                strcat(macroContent, " ");
                strcat(macroContent, token);
            }

            /* macro declared: look up in the table, if exist - error, else insert */
            if (strcmp(token, "mcro") == 0)
            {
                if (searchMacro(MACROS, token))
                {
                    /* error macro exist already */
                }
                else

                hash = mcroHash(counter, token);
                macroItem* newMacro = createMacro(token, hash);
                insertMacro(MACROS, newMacro, token, hash);

                openMacro = true; /*flag for next iterations to gather macro content*/
            }

            token = strtok(NULL, delims);
        }
    }
    
    free(macroContent);
    rewind(file);

    /* (4) the loop below re-write the source file with the macros content */
    while (fgets(line, sizeof(line), file) != NULL)
    {
        token = strtok(line, delims);
        
        while (token != NULL)
        {
            /* if token == macro/endmcro - continue */
        }
    }

}