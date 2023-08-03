#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <stdbool.h>

int decimalToBinary(int decimal);
char* binaryToBase64(const char* binary);
bool lineToIgnore(char* line);
bool isReservedWord(char* word);

#endif /* MY_UTILS_H */