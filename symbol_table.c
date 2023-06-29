#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

symbol_list* create_new_symbol_list() {
    symbol_list* list = (symbol_list*)malloc(sizeof(symbol_list));
    if (list != NULL) {
        list->head = NULL;
    }
    return list;
}

symbol_data* create_new_symbol_data(char name[], int value, enum symbol_attribute attribute) {
    symbol_data* data = (symbol_data*)malloc(sizeof(symbol_data));
    if (data != NULL) {
        strncpy(data->symbol.name, name, SYMBOL_MAX_NAME_SIZE);
        data->symbol.value = value;
        data->symbol.attribute = attribute;
        data->next = NULL;
    }
    return data;
}

void add_symbol_to_list(symbol_list* list, symbol_data* data) {
    if (list == NULL || data == NULL) {
        return;
    }
    
    if (list->head == NULL) {
        list->head = data;
    } else {
        symbol_data* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = data;
    }
}

symbol_data* find_symbol(symbol_list* list, char name[]) {
    if (list == NULL) {
        return NULL;
    }
    
    symbol_data* current = list->head;
    while (current != NULL) {
        if (strcmp(current->symbol.name, name) == 0) {
            return current;  // Symbol found
        }
        current = current->next;
    }
    
    return NULL;  // Symbol not found
}

int does_symbol_exist(symbol_list* list, symbol_data* data) {
    if (list == NULL || data == NULL) {
        return 0;
    }
    
    symbol_data* current = list->head;
    while (current != NULL) {
        if (strcmp(current->symbol.name, data->symbol.name) == 0) {
            return 1;  // Symbol exists in the list
        }
        current = current->next;
    }
    
    return 0;  // Symbol does not exist in the list
}

void free_list(symbol_list* list) {
    if (list == NULL) {
        return;
    }
    
    symbol_data* current = list->head;
    while (current != NULL) {
        symbol_data* temp = current;
        current = current->next;
        free(temp);
    }
    
    free(list);
}