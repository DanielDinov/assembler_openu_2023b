#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "globals.h"


/* all possible label types */
enum label_types { DATA=0, STRING, ENTRY, EXTERN};

typedef enum symbol_attribute_t { SYMBOL_ERROR=-1, SYMBOL_DATA=0, SYMBOL_CODE, SYMBOL_ENTRY, SYMBOL_EXTERN} symbol_attribute;
typedef struct symbol_type_t {
    char name[SYMBOL_MAX_NAME_SIZE + 1];
    int value;
    symbol_attribute attribute;
} symbol_type;
typedef struct symbol_data_type {
    symbol_type symbol;
    struct symbol_data_type* next;
} symbol_data;
typedef struct symbol_list_type {
    struct symbol_data_type* head;
} symbol_list;

void create_new_symbol_list();

symbol_data* create_new_symbol_data(char[], int, symbol_attribute);

bool add_symbol_to_list(char[], int, symbol_attribute);

symbol_data* find_symbol(char[]);

bool does_symbol_exist(symbol_data*);

void free_list();
#endif