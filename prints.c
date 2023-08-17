#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "symbol_table.h"
#include "commands.h"
#include "data_handler.h"
#include "util.h"

const char base64Lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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

char* decimalToBinaryString(int decimal) {
    int i, numBits = sizeof(int) * 8;
    char* binaryStr = (char*)malloc(numBits + 1);

    if (binaryStr == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (i = numBits - 1; i >= 0; i--) {
        binaryStr[numBits - i - 1] = ((decimal >> i) & 1) + '0';
    }
    binaryStr[numBits] = '\0';

    return binaryStr;
}

char *decimalToBinaryTwosComplement(int decimal) {

    int numBits = sizeof(int) * 8;
    int i, sign, index, carry;
    
    
    char *binary = (char *)malloc(numBits + 1); 
    
    if (binary == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    /* Start by filling the binary string with '0's*/
    for (i = 0; i < numBits; i++) {
        binary[i] = '0';
    }
    binary[numBits] = '\0'; /* Null-terminate the string */
    
    /* Determine the sign of the number */
    sign = (decimal < 0) ? -1 : 1;
    decimal *= sign;
    
    /* Convert decimal to binary using bitwise operations */
    index = numBits - 1;
    while (decimal > 0) {
        binary[index] = (decimal % 2) + '0';
        decimal /= 2;
        index--;
    }
    
    /*Apply 2's complement if the number was negative*/
    if (sign == -1) {
        carry = 1;
        index = numBits - 1;
        
        while (index >= 0) {
            if (binary[index] == '0' && carry == 1) {
                binary[index] = '1';
                carry = 0;
            } else if (binary[index] == '1' && carry == 1) {
                binary[index] = '0';
            }
            index--;
        }
    }

    return binary;
}

/*char* binaryToBase64(const char* binary) 
{
    size_t binaryLength = strlen(binary);
    size_t padding = binaryLength % 6 == 0 ? 0 : 6 - (binaryLength % 6);
    size_t base64Length = ((binaryLength + padding) / 6) * 8 + 1;

    char* base64 = (char*)malloc(base64Length * sizeof(char));
    if (base64 == NULL) {
        return NULL;
    }
    unsigned int value;
    size_t i, j;
    for (i = 0, j = 0; i < binaryLength; i += 6, j += 8) {
        value = 0;
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
}*/

/*char* decimalToBinaryString(int decimal, int numBits) {
    char* binaryStr = (char*)malloc(numBits + 1);
    int i;
    if (binaryStr == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for (i = numBits - 1; i >= 0; i--) {
        binaryStr[numBits - i - 1] = ((decimal >> i) & 1) + '0';
    }
    binaryStr[numBits] = '\0';

    return binaryStr;
}*/

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

char *intToBinaryString(int num, int numBits) {
    char *binary = (char *)malloc(numBits + 1);
    int i;
    
    if (binary == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (i = 0; i < numBits; i++) {
        binary[i] = ((num >> (numBits - i - 1)) & 1) + '0';
    }
    binary[numBits] = '\0';
    
    return binary;
}

char* decimalToBase64(int decimal) 
{
    int value = decimal;
    char temp;
    size_t j = 0;
    size_t start;
    size_t end;
    size_t base64Length = 5; 
    char* base64 = (char*)malloc(base64Length * sizeof(char));

    if (base64 == NULL) {
        return NULL;
    }

    while (value > 0) {
        base64[j++] = base64Lookup[value & 0x3F];
        value >>= 6;
    }

    base64[j] = '\0';

    start = 0;
    end = j - 1;
    while (start < end) {
        temp = base64[start];
        base64[start] = base64[end];
        base64[end] = temp;
        start++;
        end--;
    }

    return base64;
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
    for (i = 100; i < 100 + IC; i++)
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