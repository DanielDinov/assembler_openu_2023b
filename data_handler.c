#include "data_handler.h"
#include "util.h"

int DATA_IMG[MAX_DATA_SIZE];

bool add_data_num(char* str_num,int dc){
    if (dc>MAX_DATA_SIZE){
        fprintf(stderr, "Variable %s,unable to save,out of bounds exception\n", str_num);
        return false;
    }
    int num;
    num = convert_to_int(str_num);
    if (num == INT_MIN)
        return false;
    /* if here everything is fine,save number and dc will be incremented in the function caller when true is returned */
    DATA_IMG[dc] = num;
    return true;
}

int add_data_string(char* string,int dc){
    int i = 0;
    if (string[0] != '\"' || string[strlen(string) -1] != '\"'){
        fprintf(stderr, "String variable %s,incorrect format,should start and end with \"\n", string);
        return 0;
    }
    for (string++; string[0] != '\"'; string++){
        DATA_IMG[dc] = string[0];
        dc++;
        i++;
        if (dc > MAX_DATA_SIZE){
            fprintf(stderr, "String variable %s,unable to save,out of bounds exception\n",string-dc);
            return -1;
        }
    }
    string[dc] = 0; /* null terminator in int base */
    dc++;
    i++;
    return i; /* return the value of i to increment the original dc by that amount */
}