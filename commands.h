#ifndef COMMANDS_H
#define COMMANDS_H

#include "globals.h"

#define CMD_SUM 16
#define START_ADDRESS 100

cmd find_cmd(char*);

void add_machine_word(machine_word current_word, int IC);

bool add_extra_word_single_param(parameter param, bool is_source, int IC);

void add_extra_word_double_param(char* source, char* dest, int IC);

void find_parameters(parameter, parameter)

#endif