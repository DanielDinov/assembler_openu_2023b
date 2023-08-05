#include "data_handler.h"

int DATA_IMG[MAX_DATA_SIZE];

bool add_data_num(char* str_num,int dc){
    if (dc>MAX_DATA_SIZE){
        fprintf(stderr, "Line %d with variable %s,unable to save,out of bounds exception",current_line, str_num);
        return false;
    }
    int num;
    char* end_ptr;
    num = strtol(str_num,&end_ptr,10); /* convert given str to base 10 long */
    if (*end_ptr != '\0'){
        fprintf(stderr, "Line %d with variable %s,unable to convert,got %d after convertion",current_line, str_num, num);
        return false;
    }
    if (num > MAX_NUMBER|| num < MIN_NUMBER){ /* out of numbers range */
        fprintf(stderr, "Line %d with number %d,out of numbers range,max is %d and min is %d",current_line,num,MAX_NUMBER,MIN_NUMBER);
        return false;
    }
    /* if here everything is fine,save number and dc will be incremented in the function caller when true is returned */
    DATA_IMG[dc] = num;
    return true;
}

int add_data_string(char* string,int dc){
    int i = 0;
    if (string[0] != '\"' || string[strlen(string) -1] != '\"'){
        fprintf(stderr, "Line %d with string variable %s,incorrect format,should start and end with \"",current_line, string);
        return 0;
    }
    for (string++; string[0] != '\"'; string++){
        DATA_IMG[dc] = string[0];
        dc++;
        i++;
        if (dc > MAX_DATA_SIZE){
            fprintf(stderr, "Line %d with string variable %s,unable to save,out of bounds exception",current_line,string-dc);
            return -1;
        }
    }
    string[dc] = 0; /* null terminator in int base */
    dc++;
    i++;
    return i; /* return the value of i to increment the original dc by that amount */
}