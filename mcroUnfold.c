#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "util.h"
#include "macro_table.h"
#define ROW_MAX_LENGTH 81
/*TODO: add define with words like mcro,endmcro etc...*/

/* line to ignore gets a full line and check if it is a comment line or an empty line, if it is return true else false. will be transfered to util.c later */
bool lineToIgnore(char* line)
{
    /* ignore comment lines */
    if (line[0] == ';')
    {
        return true;
    }

    /* ignore empty lines */
    for (int i = 0; i < strlen(line); i++)
    {
        char ch = line[i];
        if (isspace(ch))
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

/* first read: counting amount of macros and the length of content.
 * second read: store all the macros and their content.
 * third read: write the new output file with unfolded macros.
 */

FILE* macroUnfold(FILE* file)
{
    const char delims[4] = " \n\t"; /* to ignore while tokenizing*/
    bool openMacro = false; /* will switch to true when mcro identified, return to false when encounter endmcro */
    bool addMacro = false; /* flag for mcro decleration */
    int counter = 0; /* how many macros will be in the hash table*/
    int macroLength = 0; /* count how many char to allocate for macros content */
    int nameLength = 0;
    unsigned long hash = 0; /* for hashing macros */
    macroItem* newMacro = NULL; /* pointer to handle macro items */
    char* token = NULL; /* for all tokenizing proccess */
    FILE* outputFile = fopen("originalName.am", "w"); /*TODO how to import name from input file and create a new file with AM*/
    char line[ROW_MAX_LENGTH]; /* TODO: add a checker for max 80 excluding \n */
    macroTable* MACROS = NULL;
    char* macroContent = NULL;
    char* macroName = NULL;

    /* (1) the loop below counts the amount of macros in the file to initialize and efficient table */
    while (fgets(line, ROW_MAX_LENGTH, file) != NULL)
    {
        /* ignore comment and empty lines */
        if (lineToIgnore(line)) 
        {
            continue;
        }

        /* since fgets consumes 81 chars, if none of them is a new line marker it means line is more than 80 chars*/
        char* lengthTest = strchr(line, '\n');
        if (lengthTest == NULL)
        {
            printf("Excceeding max length of line\n");
            exit(0);
        }
        
        lengthTest = NULL;
        int temp = 0; /* temp counter for macro length */
        token = strtok(line, delims);
        while (token != NULL)
        {
            if (strcmp(token, "endmcro") == 0)
            {
                if (temp > macroLength) {
                    macroLength = temp;
                }
                openMacro = false;
            }

            else if (openMacro)
            {
                temp = temp + strlen(token) + 1; /*1 for space*/
            }

            else if (addMacro)
            {
                if (strlen(token) > nameLength)
                {
                    nameLength = strlen(token);
                }
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
    }
    rewind(file);

    /* if macro found then create a table for macros and unfold (loop 2 and 3) */
    if (counter > 0)
    {
        MACROS = createMacroTable(counter);
        macroContent = (char*)malloc(macroLength * sizeof(char)); /* initializng a char array large enough to fit macro content */
        if (macroContent == NULL)
        {
            printf("memory allocation failed\n");
            exit(0);
        }
        macroName = (char*)malloc(nameLength * sizeof(char));

        /* (2) the loop below read the file and insert new macros to the hash table */
        while (fgets(line, ROW_MAX_LENGTH, file) != NULL)
        {
            if (lineToIgnore(line))
            {
                continue;
            }

            token = strtok(line, delims);
            while (token != NULL)
            {
                /* endmcro: submit the contnet in and reset the string*/
                if (strcmp(token, "endmcro") == 0)
                {
                    hash = macroHash(counter, macroName, 0);
                    newMacro = createMacro(macroName, hash, macroContent);
                    printf("createad macro succesfully\n");
                    newMacro->text = strdup(macroContent);
                    printf("macro content was added: %s\n", newMacro->text);
                    insertMacro(MACROS, newMacro, macroName, hash);
                    openMacro = false;
                    macroContent[0] = '\0';
                }

                /* openMacro: add new words to content buffer*/
                if (openMacro)
                {
                    strcat(macroContent, " ");
                    strcat(macroContent, token);
                }

                /* encounterd 'mcro' last iteration: look up in the table, if exist - error, else insert */
                if (addMacro)
                {
                    /*TODO if macro name is a reserved name (mov, jmp, etc..) print error */
                    if (searchMacro(MACROS, token))
                    {
                        printf("error: macro already exist!");
                    }
                    
                    else
                    {
                        macroName = token;
                        printf("printing macro name: %s\n", macroName);
                        openMacro = true; /*flag for next iterations to gather macro content*/
                        addMacro = false;
                    }
                }

                /* mcro declared, flag up for next iteration to consume it*/
                if (strcmp(token, "mcro") == 0)
                {
                    addMacro = true;
                }

                token = strtok(NULL, delims);
            }

            if (openMacro)
            {
                strcat(macroContent, "\n");
            }
        }
        printf("pass loop (2)\n\n");/*tester remove*/
        /*free(macroContent);*/
        rewind(file);
        /* (3) the loop below re-write the source file with the macros content */
        while (fgets(line, ROW_MAX_LENGTH, file) != NULL)
        {
            token = strtok(line, delims);
            
            if (lineToIgnore(line)) 
            {
                continue;
            }

            while (token != NULL)
            {
                if (openMacro)
                {
                    if (strcmp(token, "endmcro") == 0)
                    {
                        openMacro = false;
                    }
                    token = strtok(NULL, delims);
                    continue;
                }
                else
                    /*unfold existing macro*/
                printf("token in loop: %s\n", token);/*tester remove*/
                if (searchMacro(MACROS, token))
                {
                    printf("found this in macro table: %s\n", token);
                }
                if (!searchMacro(MACROS, token))
                {
                    printf("didnt find this in macro table: %s\n", token);
                }
                newMacro = getMacro(MACROS, token);
                
                if (newMacro != NULL)
                {
                    printf("try to unfold mcro\n");/*tester remove*/
                    printf("%s\n", newMacro->text);
                    fprintf(outputFile, "%s", newMacro->text);
                    newMacro = NULL;
                    token = strtok(NULL, delims);
                    continue;
                }

                /* skipping mcro flags*/
                else if (strcmp(token, "mcro") == 0)
                {
                    openMacro = true;
                    token = strtok(NULL, delims);
                    continue;
                }

                /* copy paste text from source file to AM file*/
                else
                {
                    fprintf(outputFile, "%s", token);
                    fprintf(outputFile, "%s", " ");
                    token = strtok(NULL, delims);
                }
            }
            fprintf(outputFile, "%s", "\n");
        }
    } /* end of case macro exist in source file */
    
    /* if no macros - go to loop 3 and make AM file with no comment lines and empty lines */
    else
    {
        while (fgets(line, ROW_MAX_LENGTH, file) != NULL)
        {
            token = strtok(line, delims);
            /* skip comment or empty line */
            if (lineToIgnore(line)) 
            {
                continue;
            }

            while (token != NULL)
            {
                fprintf(outputFile, "%s", token);
                fprintf(outputFile, "%s", " ");
                token = strtok(NULL, delims);
            }
            fprintf(outputFile, "%s", "\n");
        }

    }
    return outputFile;
}

int main(int argc, char* argv[])
{
    FILE* file = fopen(argv[1], "r+");

    if (file == NULL)
    {
        printf("Failed to open file: %s\n", argv[1]);
    }

    FILE* amOutputFile = macroUnfold(file);
    return 1;
}