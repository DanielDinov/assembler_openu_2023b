#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

symbol_data* symbol_list;

void create_new_symbol_list() {
    symbol_data* head = create_new_symbol_data("", -1, SYMBOL_ERROR);
    if (head != NULL) {
        symbol_list = head;
    }
}

symbol_data* create_new_symbol_data(char name[], int value, symbol_attribute attribute) {
    symbol_data* data = (symbol_data*)malloc(sizeof(symbol_data));
    if (data == NULL) {
        fprintf(stderr, "line %d unable to allocate memory",current_line);
        return NULL;
    }
    strncpy(data->symbol.name, name, SYMBOL_MAX_NAME_SIZE);
    data->symbol.value = value;
    data->symbol.attribute = attribute;
    data->next = NULL;
    return data;
}

bool add_symbol_to_list(char name[], int value, symbol_attribute attribute) {
    symbol_data* data = create_new_symbol_data(name,value,attribute);
    symbol_data* current;
    if (symbol_list == NULL || data == NULL) {
        return false;
    }
    if (does_symbol_exist(data)){
        fprintf(stderr, "line %d symbol already exists",current_line)
        return false;
    }
    if (symbol_list->head == NULL) {
        symbol_list->head = data;
    } else {
        current = symbol_list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = data;
    }
    return true
}

symbol_data* find_symbol(char name[]) {
    symbol_data* current = symbol_list->head;
    if (symbol_list == NULL) {
        return NULL;
    }
    while (current != NULL) {
        if (strcmp(current->symbol.name, name) == 0) {
            return current;  // Symbol found
        }
        current = current->next;
    }
    
    return NULL;  // Symbol not found
}

bool does_symbol_exist(symbol_data* data) {
    symbol_data* current = symbol_list->head;
    if (symbol_list == NULL || data == NULL) {
        return false;
    }
    while (current != NULL) {
        if (strcmp(current->symbol.name, data->symbol.name) == 0) {
            return true;  // Symbol exists in the list
        }
        current = current->next;
    }
    
    return false;  // Symbol does not exist in the list
}

void free_list() {
    symbol_data* current = symbol_list->head;
    symbol_data* temp;
    if (symbol_list == NULL) {
        return;
    }
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    
    free(symbol_list);
}