#ifndef COMMANDS_H
#define COMMANDS_H

#include "globals.h"

#define CMD_SUM 16
#define START_ADDRESS 100

int CODE_IMG[MAX_DATA_SIZE];

/* return a valid cmd by name if found else null */
cmd* find_cmd(char*);

/* inserts a machine word into the correct location */
void add_machine_word(machine_word current_word, int IC);

/* adds an extra word with only a single param */
bool add_extra_word_single_param(parameter param, bool is_source, int IC, char* fileName);

/* add a word with both params,for register */
void add_extra_word_double_param(char* source, char* dest, int IC);

/* inserts into the arguments the correct parameter according to the line */
void find_parameters(parameter* first_param, parameter* second_param);

/* returns the IC */
int getIC();

/* add to internal variable the amount after pass */
void addIC(int counter);

#endif