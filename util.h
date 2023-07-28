#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <stdbool.h>

bool read_line(char* line);
int decimalToBinary(int decimal);
char* binaryToBase64(const char* binary);
void mcroUnfold(FILE* file);
unsigned int mcroHash(int n, const char* name);


#endif /* MY_UTILS_H */