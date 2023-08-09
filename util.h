#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <stdbool.h>
#define ROW_MAX_LENGTH 82 /*80 + \n*/

int decimalToBinary(int decimal);
char* binaryToBase64(const char* binary);
bool lineToIgnore(char* line);
bool isReservedWord(char* word);
bool is_register(char* word);
int isDirective(char* word);
char* str_allocate_cat(char* first_str, char* second_str);
const char delims[4];

#endif /* MY_UTILS_H */