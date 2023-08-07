#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include "globals.h"

/* adds number to data_img and return true/false according to sucess */
bool add_data_num(char*,int);

/* adds string and also null terminator to data_img and return the number of incerement for dc,0 in case of invalid format and -1 in case of out of bounds */
int add_data_string(char*,int);

#endif