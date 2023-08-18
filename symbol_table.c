#include "symbol_table.h"

/* symbol table implements linked list to store and manage symbols */

symbol_list* symbol_list_instance = NULL;

void create_new_symbol_list() {
    symbol_list_instance = (symbol_list*)malloc(sizeof(symbol_list));
    if (symbol_list_instance != NULL) {
        symbol_list_instance->head = NULL;
    } else {
        fprintf(stderr, "Unable to allocate memory for symbol_list_instance\n");
    }
}

symbol_data* create_new_symbol_data(char name[], int value, symbol_attribute attribute) {
    symbol_data* data = (symbol_data*)malloc(sizeof(symbol_data));
    if (data == NULL) {
        fprintf(stderr, "Unable to allocate memory for symbol\n");
        return NULL;
    }
    strncpy(data->symbol.name, name, SYMBOL_MAX_NAME_SIZE);
    data->symbol.value = value;
    data->symbol.attribute = attribute;
    data->next = NULL;
    return data;
}

bool add_symbol_to_list(char name[], int value, symbol_attribute attribute) {
    symbol_data* data;
    symbol_data* current;
    if (symbol_list_instance == NULL) {
        return false;
    }
    data = create_new_symbol_data(name,value,attribute);
    if (data == NULL) {
        return false;
    }
    if (does_symbol_exist(data)){
        fprintf(stderr, "symbol %s already exists\n",data->symbol.name);
        return false;
    }
    if (symbol_list_instance->head == NULL) {
        symbol_list_instance->head = data;
    } else {
        current = symbol_list_instance->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = data;
    }
    return true;
}

symbol_data* find_symbol(char name[]) {
    symbol_data* current;
    if (symbol_list_instance == NULL) {
        return NULL;
    }
    current = symbol_list_instance->head;
    while (current != NULL) {
        if (strcmp(current->symbol.name, name) == 0) {
            return current;  /* Symbol found */
        }
        current = current->next;
    }
    
    return NULL;  /* Symbol not found */
}

bool does_symbol_exist(symbol_data* data) {
    symbol_data* current;
    if (symbol_list_instance == NULL || data == NULL) {
        return false;
    }
    current = symbol_list_instance->head;
    while (current != NULL) {
        if (strcmp(current->symbol.name, data->symbol.name) == 0) {
            return true;  /* Symbol exists in the list */
        }
        current = current->next;
    }
    
    return false;  /* Symbol does not exist in the list */
}

void free_list() {
    symbol_data* current;
    symbol_data* temp;
    if (symbol_list_instance == NULL) {
        return;
    }
    current = symbol_list_instance->head;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }

    free(symbol_list_instance);
}