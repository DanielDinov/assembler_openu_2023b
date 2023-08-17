#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "globals.h"


char* am_extension = ".am";
char* as_extension = ".as";
char* entry_extension = ".ent";
char* external_extension = ".ext";
char* object_extension = ".ob";

const char* saved_word_strings[] =
{
        "mov",
        "cmp",
        "add",
        "sub",
        "not",
        "clr",
        "lea",
        "inc",
        "dec",
        "jmp",
        "bne",
        "red",
        "prn",
        "jsr",
        "rts",
        "stop",
        ".data",
        ".string",
        ".entry",
        ".extern",
        "r0",
        "r1",
        "r2",
        "r3",
        "r4",
        "r5",
        "r6",
        "r7"
};

const char* directives[] =
{
    ".date",/*1*/
    ".string",/*2*/
    ".entry",/*3*/
    ".extern"/*4*/
};

const char* registers[] =
{
    "@r0",
    "@r1",
    "@r2",
    "@r3",
    "@r4",
    "@r5",
    "@r6",
    "@r7"
};

bool lineToIgnore(char* line)
{
    int i;

    /* ignore comment lines */
    if (line[0] == ';')
    {
        return true;
    }
    /* ignore empty lines */
    for (i = 0; i < strlen(line); i++)
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

bool isReservedWord(char* word)
{
    int i;
    for (i = 0; i < sizeof(saved_word_strings) / sizeof(saved_word_strings[0]); i++)
    {
        if (strcmp(word, saved_word_strings[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool is_register(char* word) {
    int i;
    for (i = 0; i < sizeof(registers) / sizeof(registers[0]); i++) {
        if (strcmp(word, registers[i]) == 0){
            return true;
        }
    }
    return false;
}

int isDirective(char* word)
{
    int i;
    for (i = 0; i < sizeof(directives) / sizeof(directives[0]); i++)
    {
        if (strcmp(word, directives[i]) == 0)
        {
            return i+1;
        }
    }
    return 0;
}

char* str_allocate_cat(char* first_str, char* second_str) 
{
    char* str = (char*)malloc(strlen(first_str) + strlen(second_str) + 1);
    if (str == NULL)
    {
        printf("Memory allocation failed\n");
        exit(0);
    }
    strcpy(str, first_str);
    strcat(str, second_str);
    return str;
}

void format_line(char* word) {
    int i, buffer_index = 0, in_word = 0;
    char line[MAX_LINE_LEN];
    for (i = 0; i < strlen(word); i++) {
        if (word[i] == ' ' || word[i] == '\t' || word[i] == '\r') {
            if (in_word) {
                line[buffer_index++] = ' ';
                in_word = 0;
            }
            continue;
        }
        /* space the built string correctly */
        if (word[i] == ',') {
            line[buffer_index++] = ' ';
            line[buffer_index++] = ',';
            line[buffer_index++] = ' ';
            continue;
        }
        line[buffer_index++] = word[i];
        in_word = 1;
    }
    /* end string with null terminator */
    line[buffer_index-1] = '\0';
    strcpy(word, line);
}

int convert_to_int(char* word){
    char* end_ptr;
    int num;
    char* str_num = word;
    while (word[0] == ' ')
        word++;
    if (word[0] == '\0') {
        fprintf(stderr, "Input is empty or contains only spaces\n");
        return INT_MIN;
    }
    if(word[0] == '-')
        str_num = word + 1;
    num = strtol(str_num,&end_ptr,10); /* convert given str to base 10 long */
    if(word[0] == '-')
        num = num*-1;
    if (*end_ptr != '\0' && !isspace((unsigned char)*end_ptr)){
        printf("end_ptr:%s",end_ptr);
        fprintf(stderr, "Unable to convert %s,got %d after convertion\n", word, num);
        return INT_MIN;
    }
    if (num > MAX_NUMBER|| num < MIN_NUMBER){ /* out of numbers range */
        fprintf(stderr, "Number %d,out of numbers range,max is %d and min is %d\n",num,MAX_NUMBER,MIN_NUMBER);
        return INT_MIN;
    }
    return num;
}

const char delims[4] = " \t\n"; /* to ignore while tokenizing */