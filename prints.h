#ifndef PRINTS_H
#define PRINTS_H

#include "globals.h"
#include "symbol_table.h"

const char base64Lookup[65];

void write_entry_file (symbol_type symbol, char* fileName);
void write_external_file (char* symbol_name,int word_location, char* fileName);
char* decimalToBinaryString(int decimal);
char* binaryToBase64(const char* binary);
char* decimalToBase64(int decimal);
void printOBJ(char* file_name);

#endif