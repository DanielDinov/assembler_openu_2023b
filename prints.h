#ifndef PRINTS_H
#define PRINTS_H

#include "globals.h"
#include "symbol_table.h"

void write_entry_file (symbol_type symbol, char* fileName);

void write_external_file (char* symbol_name,int word_location, char* fileName);

void printOBJ(char* file_name);

void removeOutputs(char* fileName);

#endif