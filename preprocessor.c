#include "util.h"
#include "macro_table.h"
#include "globals.h"

/* preprocessor gets a file name, read through the .as file and creates a formated .am file
   if macros a initialized it will create a macro table to store them and later unfolds it into the .am file
   if no errors found it returns true, else false*/

bool preprocessor (char* file_name)
{
    bool success_flag = true, open_macro = false, add_macro = false, skip = true, first_word = true;
    int counter = 0, macro_length = 0, name_length = 0, current_line = 1, temp = 0;
    char *token = NULL, *macro_content = NULL, *macro_name = NULL;
    char line[MAX_LINE_LEN + 3];
    macroTable* macro_table = NULL;
    macroItem* macro_item = NULL;
    char* source_file_name = str_allocate_cat(file_name, as_extension);
    char* output_file_name = str_allocate_cat(file_name, am_extension);

    /* add .as  and .am extension and open source and output file */
    FILE* source_file = fopen(source_file_name, "r");
    FILE* output_file = fopen(output_file_name, "w");
    if (source_file == NULL)
    {
        printf("Failed to open source file: %s in preprocessor stage.\n", source_file_name);
        success_flag = false;
        free(source_file_name);
        free(output_file_name);
        return success_flag;
    }
    if (output_file == NULL)
    {
        printf("Failed to create the am file: %s\n", output_file_name);
        success_flag = false;
        free(source_file_name);
        free(output_file_name);
        fclose(source_file);
        return success_flag;
    }
    free(source_file_name);
    free(output_file_name);

    /* first loop counts macros and their lengths to allocate efficient table */
    while(fgets(line, MAX_LINE_LEN + 3, source_file) != NULL)
    {
        if (!lengthTest(line))
        {
            printf("ERROR: line %d is over 80 char\n", current_line);
            /* do exist, buffers overflow and or memory leaks risk */
            exit(1);
        }
        if (lineToIgnore(line)) 
        {
            current_line++;
            continue;
        }

        token = strtok(line, delims);
        while (token)
        {
            if (strcmp(token, "endmcro") == 0)
            {
                if (temp > macro_length)
                    macro_length = temp;
                temp = 0;
                open_macro = false;
            }
            else
            if (open_macro)
            {
                temp = temp + strlen(token) + 2;
            }
            else
            if (add_macro)
            {
                if (isReservedWord(token))
                {
                    printf("ERROR:at line %d Macro name is illegal (%s is a reserved word)\n", current_line, token);
                    success_flag = false;
                }
                if (strlen(token)+2 > name_length)
                    name_length = strlen(token) + 2;
                counter++;
                add_macro = false;
                open_macro = true;
            }
            else if (strcmp(token, "mcro") == 0)
            {
                add_macro = true;
            }
            
            token = strtok(NULL, delims);
        }
        current_line++;
    }
    /* reset for 2nd loop */
    rewind (source_file);
    token = NULL;
    current_line = 1;

    if (counter > 0)
    {
        macro_table = createMacroTable(counter);
    }

    /* 2nd loop gets the macros, insert them to table and unfold */
    while(fgets(line, MAX_LINE_LEN + 3, source_file) != NULL)
    {
        format_line(line);
        first_word = true;
        if (lineToIgnore(line)) 
        {
            current_line++;
            skip = true;
            continue;
        }
        token = strtok(line, delims);
        while(token)
        {
            if (counter > 0 && (macro_item = getMacro(macro_table, token)) != NULL)
            {
                fprintf(output_file, "%s", macro_item->text);
                skip = true;
                token = strtok(NULL, delims);
            }
            else
            /* endmcro: submit the contnet in and reset the string*/
            if ((strcmp(token, "endmcro") == 0) && counter > 0)
            {
                macro_item = createMacro(macro_name, macro_content);
                insertMacro(macro_table, macro_item);
                open_macro = false;
                free(macro_name);
                macro_name = NULL;
                free(macro_content);
                macro_content = NULL;
                macro_item = NULL;
                skip = true;
                token = strtok(NULL, delims);
            }
            else
            /* openMacro: add new words to content buffer*/
            if (open_macro && counter > 0)
            {
                if (macro_content == NULL)/*first word in macro content */
                {
                    macro_content = (char*)malloc(macro_length * sizeof(char));
                    first_word = false;
                    strcpy(macro_content, token);
                    skip = true;
                    token = strtok(NULL, delims);
                }
                else
                {
                    if (!first_word){
                        strcat(macro_content, " ");
                        first_word = false;}
                    strcat(macro_content, token);
                    first_word = false;
                    skip = true;
                    token = strtok(NULL, delims); 
                }
            }
            else
            /* encounterd 'mcro' last iteration: look up in the table, if exist - error, else insert to table */
            if (add_macro && counter > 0)
            {
                if (searchMacro(macro_table, token))
                {
                    printf("ERROR: in line %d macro name \"%s\" already exist!\n", current_line, token);
                    success_flag = false;
                    token = strtok(NULL, delims);
                }
                else
                {
                    macro_name = (char*)malloc(name_length * sizeof(char));
                    strcpy(macro_name, token);
                    open_macro = true; /*flag for next iterations to gather macro content*/
                    add_macro = false;
                    skip = true;
                    first_word = false;
                    token = strtok(NULL, delims);
                }
            }
            else
            /* mcro declared, flag up for next iteration to consume it*/
            if ((strcmp(token, "mcro") == 0) && counter > 0)
            {
                first_word = false;
                add_macro = true;
                skip = true;
                token = strtok(NULL, delims);
            }
            /* no macro handling - copy paste to new file */
            else
            {
                if (first_word){
                    first_word = false;
                    fprintf(output_file, "%s", token);
                    skip = false;
                    token = strtok(NULL, delims);}
                else{
                    fprintf(output_file, " %s", token);
                    skip = false;
                    token = strtok(NULL, delims);}
            }
        }

        current_line++;
        if (open_macro && counter > 0 && macro_content != NULL) /*meaning printed all the macro content and a new line \n*/
        {
                strcat(macro_content, "\n");
        }
        else
        if(!skip)
        {
            fprintf(output_file, "%s", "\n");
        }
    }


    if (counter > 0)
    {
        freeMacroTable(macro_table);
    }
    fclose(source_file);
    fclose(output_file);
    
    return success_flag;
}