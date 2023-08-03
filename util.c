#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/*
 *
 */

const char base64Lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char* op_code_strings[] = {
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
        "stop"
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
    for (int i = 0; i < sizeof(op_code_strings) / sizeof(op_code_strings[0]); i++)
    {
        if (strcmp(word, op_code_strings[i]) == 0)
        {
            return true;
        }
    }
    return false;
}