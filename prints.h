#ifndef PRINTS_H
#define PRINTS_H

#include "globals.h"
#include "symbol_table.h"

/* writes data to entry file */
void write_entry_file (symbol_type symbol, char* fileName);

/* writes data to external file */
void write_external_file (char* symbol_name,int word_location, char* fileName);

/* write to obj file */
void printOBJ(char* file_name);

/* frees dynamical allocations */
void removeOutputs(char* fileName);

#endif