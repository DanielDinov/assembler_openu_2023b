#ifndef MY_UTILS_H
#define MY_UTILS_H

#include "globals.h"

char* am_extension;
char* as_extension;
char* entry_extension;
char* external_extension;
char* object_extension;

/* checks is line is meant to be ignored or not */
bool lineToIgnore(char* line);

/* check for len */
bool lengthTest(char* line);

/* checks if word is reserved word for assembly */
bool isReservedWord(char* word);

/* check if word is an register */
bool is_register(char* word);

/* dynamically allocated space and combines two strings */
char* str_allocate_cat(char* first_str, char* second_str);

/* formats ilne to be able to strtok correctly */
void format_line(char*);

/* converts char* to int and validates the input */
int convert_to_int(char* word);

const char delims[4];

#endif