#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <util.h>

/*
 *
 */

const char base64Lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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

int decimalToBinary(int decimal)
{
    int binary = 0;
    int base = 1;

    while (decimal > 0) 
    {
        int remainder = decimal % 2;
        binary += remainder * base;
        decimal /= 2;
        base *= 10;
    }

    return binary;
}

char* binaryToBase64(const char* binary) 
{
    size_t binaryLength = strlen(binary);
    size_t padding = binaryLength % 6 == 0 ? 0 : 6 - (binaryLength % 6);
    size_t base64Length = ((binaryLength + padding) / 6) * 8 + 1;

    char* base64 = (char*)malloc(base64Length * sizeof(char));
    if (base64 == NULL) {
        return NULL;
    }

    size_t i, j;
    for (i = 0, j = 0; i < binaryLength; i += 6, j += 8) {
        unsigned int value = 0;
        for (size_t k = 0; k < 6; k++) {
            value <<= 1;
            if (i + k < binaryLength && binary[i + k] == '1') {
                value |= 1;
            }
        }

        base64[j] = base64Lookup[value >> 2];
        base64[j + 1] = base64Lookup[(value << 4) & 0x3F];
    }

    for (i = 0; i < padding; i++) {
        base64[base64Length - 2 - i] = '=';
    }

    base64[base64Length - 1] = '\0';
    return base64;
}

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

bool isReservedWord(char* word)
{
    for (int i = 0; i < sizeof(saved_word_strings) / sizeof(saved_word_strings[0]); i++)
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
    for (int i = 0; i < sizeof(registers) / sizeof(registers[0]); i++) {
        if (strcmp(word, registers[i]) == 0)
            return true;
    }
    return false;
}

int isDirective(char* word)
{
    for (int i = 0; i < sizeof(directives) / sizeof(directives[0]); i++)
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
        if (word[i] == ' ' || word[i] == '\t') {
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
            continue;
        }
        line[buffer_index++] = word[i];
        in_word = 1;
    }
    /* end string with null terminator */
    line[buffer_index] = '\0';
    strcpy(word, line)
}

const char delims[4] = " \n\t"; /* to ignore while tokenizing*/