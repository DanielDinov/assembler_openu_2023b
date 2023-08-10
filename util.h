#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <stdbool.h>
#include <globals.h>

int decimalToBinary(int decimal);
char* binaryToBase64(const char* binary);
bool lineToIgnore(char* line);
bool isReservedWord(char* word);
bool is_register(char* word);
int isDirective(char* word);
char* str_allocate_cat(char* first_str, char* second_str);
void format_line(char*);
const char delims[4];

#endif /* MY_UTILS_H */