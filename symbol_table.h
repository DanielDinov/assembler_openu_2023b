
#define SYMBOL_MAX_NAME_SIZE 31

/* all possible label types */
enum label_types { DATA=0, STRING, ENTRY, EXTERN};


enum symbol_attribute { SYMBOL_DATA=0, SYMBOL_CODE, SYMBOL_ENTRY, SYMBOL_EXTERN};

struct symbol_type {
    char name[SYMBOL_MAX_NAME_SIZE];
    int value;
    symbol_attribute attribute;
};

typedef struct symbol_data_type {
    symbol_type symbol;
    symbol_data_type* next;
} symbol_data;

typedef struct symbol_list_type {
    symbol_data* head;
} symbol_list;

symbol_list* create_new_symbol_list();

symbol_data* create_new_symbol_data(char[], int, symbol_attribute);

void add_symbol_to_list(symbol_list*, symbol_data*);

symbol_data* find_symbol(char[]);

int does_symbol_exist(symbol_list*,symbol_data*);

void free_list(symbol_list*);
