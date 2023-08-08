#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <globals.h>

#define ROW_MAX_LENGTH 82 /*80 + \n*/    /* can use MAX_LINE_LEN from globals.h */

int decimalToBinary(int decimal);
char* binaryToBase64(const char* binary);
bool lineToIgnore(char* line);
bool isReservedWord(char* word);
int isDirective(char* word);
char* str_allocate_cat(char* first_str, char* second_str);
void format_line(char*);
const char delims[4] = " \n\t"; /* to ignore while tokenizing*/

#endif /* MY_UTILS_H */