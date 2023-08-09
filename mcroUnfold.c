#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "util.h"
#include "macro_table.h"
#define MAX_LINE_LEN 80

/* TODO fix issue where empty line appears above unfolded macros - maybe print RAW file and then eliminate 
   is it coming from the macroContent itself or printing commands?*/
/* calling macro can only come after defining it (page 38 pdf) */

/* first read: counting amount of macros and the length of content.
 * second read: store all the macros and their content.
 * third read: write the new output file with unfolded macros. */

FILE* macroUnfold(FILE* file, char* fileName)
{
    char* am_extension = ".am";
    bool openMacro = false, addMacro = false; /* flags for macro handling */
    bool skip = false; /* for printing loop */
    int counter = 0, macroLength = 0, nameLength = 0, temp = 0, currentLine = 1;  /* counters for macro amount and length */
    unsigned long hash = 0; /* for hashing macros */
    macroTable* MACROS = NULL;
    macroItem* newMacro = NULL; /* pointer to handle macro items */
    char *token = NULL, *macroContent = NULL, *macroName = NULL;
    char line[MAX_LINE_LEN+2];
    char* outputFileName = str_allocate_cat(fileName, am_extension); /* file name argument comes with no proper extension, fix it before open file */

    printf("%s\n", outputFileName);
    FILE* outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL)
    {
        printf("Failed to open file: %s\n", outputFileName);
    }

    /* (1) the loop below counts the amount of macros in the file to initialize and efficient table */
    while (fgets(line, MAX_LINE_LEN+2, file) != NULL)
    {
        /* ignore comment and empty lines */
        if (lineToIgnore(line)) 
        {
            currentLine++;
            continue;
        }

        /* since fgets consumes 81 chars, if none of them is a new line marker it means line is more than 80 chars*/
        char* lengthTest = strchr(line, '\n');
        if (lengthTest == NULL)
        {
            printf("ERROR: Excceeding max length of line at line: %d\n", currentLine);
        }
        
        lengthTest = NULL;
        token = strtok(line, delims);
        while (token != NULL)
        {
            if (strcmp(token, "endmcro") == 0)
            {
                if (temp > macroLength) {
                    macroLength = temp;
                }
                temp = 0;
                openMacro = false;
            }

            else if (openMacro)
            {
                temp = temp + strlen(token) + 3; /*2 for space, \n etc.*/
            }

            else if (addMacro)
            {
                if (isReservedWord(token))
                {
                    printf("ERROR:at line %d Macro name is illegal (%s is a reserved word)\n", currentLine, token);
                    exit(0);
                }
                if (strlen(token) > nameLength)
                    nameLength = strlen(token);
                addMacro = false;
                openMacro = true;
            }

            else if (strcmp(token, "mcro") == 0)
            {
                addMacro = true;
                counter++;
            }

            token = strtok(NULL, delims);
        }
        currentLine++;
    }

    rewind(file);
    currentLine = 1;

    printf("finished (1), counter: %d, macroLength: %d\n", counter, macroLength);/*tester remove*/
    /* if macro found then create a table for macros and unfold (loop 2 and 3) */
    if (counter > 0) 
    {
        MACROS = createMacroTable(counter);
    }
    /* (2) the loop below read the file and insert new macros to the hash table */
        
    while (fgets(line, MAX_LINE_LEN + 2, file) != NULL)
    {
        if (lineToIgnore(line))
        {
            currentLine++;
            continue;
        }

        token = strtok(line, delims);
        while (token != NULL)
        {
            /* endmcro: submit the contnet in and reset the string*/
            if ((strcmp(token, "endmcro") == 0) && counter > 0)
            {
                hash = macroHash(MACROS->size, macroName, 0);
                newMacro = createMacro(macroName, hash, macroContent);
                strcpy(newMacro->text, macroContent);
                insertMacro(MACROS, newMacro, macroName, hash);
                openMacro = false;
                free(macroName);
                macroName = NULL;
                free(macroContent);
                macroContent = NULL;
            }

            /* openMacro: add new words to content buffer*/
            if (openMacro && counter > 0)
            {
                if (macroContent == NULL)
                {
                    macroContent = (char*)malloc(macroLength * sizeof(char));
                    if (macroContent == NULL)
                    {
                        printf("macroContent memory allocation failed\n");
                        exit(0);
                    }
                    strcpy(macroContent, token);
                    strcat(macroContent, " ");
                }
                else
                {
                    strcat(macroContent, token);
                    strcat(macroContent, " ");
                }
            }
                /* encounterd 'mcro' last iteration: look up in the table, if exist - error, else insert */
            if (addMacro && counter > 0)
            {
                if (searchMacro(MACROS, token))
                {
                    printf("ERROR: in line %d macro name \"%s\" already exist!\n", currentLine, token);
                    exit(0);
                }

                else
                {
                    nameLength = strlen(token) + 1;
                    macroName = (char*)malloc(nameLength * sizeof(char));
                    strcpy(macroName, token);
                    openMacro = true; /*flag for next iterations to gather macro content*/
                    addMacro = false;
                }
            }

            /* mcro declared, flag up for next iteration to consume it*/
            if ((strcmp(token, "mcro") == 0) && counter > 0)
            {
                addMacro = true;
            }

            token = strtok(NULL, delims);
        }
            currentLine++;

        if (openMacro && counter > 0)
        {
            if (macroContent != NULL)
                strcat(macroContent, "\n");
        }
    }

    printf("finished loop (2)\n");
    rewind(file);
    currentLine = 1;

        /* (3) the loop below re-write the source file with the macros content */
    while (fgets(line, MAX_LINE_LEN + 2, file) != NULL)
    {
        printf("in loop 3\n");
        token = strtok(line, delims);

        if (lineToIgnore(line))
        {
            printf("ignored line %d\n", currentLine);
            currentLine++;
            skip = true;
            token = strtok(NULL, delims);
            continue;
        }

        while (token != NULL)
        {
            printf("token: %s\n", token);
            if (openMacro && counter > 0)
            {
                if (strcmp(token, "endmcro") == 0)
                {
                    openMacro = false;
                }
                token = strtok(NULL, delims);
                skip = true;
                continue;
            }
            else

                /*unfold existing macro*/
            if (counter > 0)
            {
                newMacro = getMacro(MACROS, token);
            }

            if (newMacro != NULL)
            {
                printf("found macro: %s\nhash: %lu\ncontent:\n%s\n", newMacro->key, newMacro->hashValue, newMacro->text);
                fprintf(outputFile, "%s", newMacro->text);
                newMacro = NULL;
                token = strtok(NULL, delims);
                continue;
            }

            /* skipping mcro flags*/
            else if ((strcmp(token, "mcro") == 0) && counter > 0)
            {
                openMacro = true;
                token = strtok(NULL, delims);
                skip = true;
                continue;
            }

            /* copy paste text from source file to AM file*/
            else
            {
                skip = false;
                fprintf(outputFile, "%s", token);
                fprintf(outputFile, "%s", " ");
                token = strtok(NULL, delims);
            }
        }
        printf("finished line %d\n", currentLine);
        currentLine++;
        if (!skip)
        {
            fprintf(outputFile, "%s", "\n");
            skip = false;
        }
    }
    if (counter > 0)
    {
        freeMacroTable(MACROS);
        printf("freed macro table\n");
        /*todo free macro name*/
    }

    free(outputFileName);
    return outputFile;
}

int main(int argc, char* argv[])
{
    char* fileName; /* the file name as given as argument */
    char* fileNameToOpen = NULL; /* modified with .as */
    char* as_extension = ".as";

    if (argc < 2)
    {
        printf("Usage: %s <filename> <filename> ...\n", argv[0]);
    }
    /* Processing each file name argument */
    for (int i = 1; i < argc; i++)
    {
        fileName = argv[i];
        fileNameToOpen = str_allocate_cat(fileName, as_extension);
        FILE* file = fopen(fileNameToOpen, "r+");

        if (file == NULL)
        {
            printf("Failed to open file: %s\n", fileName);
            free(fileNameToOpen);
            fileNameToOpen = NULL;
            continue; /* Move on to the next file */
        }

        FILE* amOutputFile = macroUnfold(file, fileName); /* unfold macros, return new file with .am extension */
        free(fileNameToOpen);
        fileNameToOpen = NULL;
        fclose(file);
        fclose(amOutputFile);
    }

    return 0;

}