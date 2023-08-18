#include <stdint.h>
#include "globals.h"
#include "symbol_table.h"
#include "commands.h"
#include "data_handler.h"
#include "util.h"

/* prints bundles the printing to files functions */

void write_entry_file (symbol_type symbol, char* fileName)
{
    char* file_name_to_open = str_allocate_cat(fileName, entry_extension);
    FILE* file = fopen(file_name_to_open, "a");
    if (file == NULL)
    {
        printf("ERROR: unable to write into %s\n", file_name_to_open);
        free(file_name_to_open);
        exit(0);
    }
    if (ftell(file) != 0)
        fprintf(file, "\n");
    fprintf(file, "%s\t%d", symbol.name, symbol.value);
    fclose(file);
    free(file_name_to_open);
}

void write_external_file (char* symbol_name,int word_location, char* fileName)
{
    char* file_name_to_open = str_allocate_cat(fileName, external_extension);
    FILE* file = fopen(file_name_to_open, "a");
    if (file == NULL)
    {
        printf("ERROR: unable to write into %s\n", file_name_to_open);
        free(file_name_to_open);
        exit(0);
    }
    if (ftell(file) != 0)
        fprintf(file, "\n");
    fprintf(file, "%s\t%d", symbol_name, word_location);
    fclose(file);
    free(file_name_to_open);
}

char* convertToBase64(uint16_t binaryData)
{
    const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static char base64String[2];
    binaryData &= 0x0FFF;

    base64String[0] = base64_table[(binaryData >> 6) & 0x3F];
    base64String[1] = base64_table[binaryData & 0x3F];
    base64String[2] = '\0';

    return base64String;
}

void printOBJ(char* file_name)
{
    int i;
    int IC = getIC();
    int DC = getDC();
    char* b64;
    char* object_file_name = str_allocate_cat(file_name, object_extension);
    FILE* file = fopen (object_file_name, "w");

    if (file == NULL)
    {
        printf("ERROR: failed to create %s\n", object_file_name);
        free(object_file_name);
        return;
    }

    fprintf(file, "%d %d", IC, DC);
    for (i = INITIAL_ADDRESS; i < INITIAL_ADDRESS + IC; i++)
    {
        b64 = convertToBase64(CODE_IMG[i]);
        fprintf(file, "\n%s", b64);
    }
    for (i = 0; i < DC; i++)
    {
        b64 = convertToBase64(DATA_IMG[i]);
        fprintf(file, "\n%s", b64);
    }
    free(object_file_name);
    fclose(file);
}

/* in case of failure at any stage - remove .ext and .ent as instructed. check if files exist first. */
void removeOutputs(char* fileName)
{
    FILE* file = NULL;
    char* am_file = str_allocate_cat(fileName, am_extension);
    char* entry_file = str_allocate_cat(fileName, entry_extension);
    char* extern_file = str_allocate_cat(fileName, external_extension);
    if ((file = fopen(am_file, "r")) != NULL)
        remove(am_file);
    free(am_file);
    file = NULL;
    if ((file = fopen(entry_file, "r")) != NULL)
        remove(entry_file);
    free(entry_file);
    file = NULL;
    if ((file = fopen(extern_file, "r")) != NULL)
        remove(extern_file);
    free(extern_file);
}