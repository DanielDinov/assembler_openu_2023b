#ifndef MY_UTILS_H
#define MY_UTILS_H

#include "globals.h"
#include "symbol_table.h"

char* am_extension;
char* as_extension;
char* entry_extension;
char* external_extension;

int decimalToBinary(int decimal);
char* decimalToBase64(int decimal);
bool lineToIgnore(char* line);
bool isReservedWord(char* word);
bool is_register(char* word);
int isDirective(char* word);
char* str_allocate_cat(char* first_str, char* second_str);
void format_line(char*);
void write_entry_file (symbol_type symbol, char* fileName);
void write_external_file (char* symbol_name,int word_location, char* fileName);
int convert_to_int(char* word);
const char delims[4];

#endif