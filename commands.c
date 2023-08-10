#include "commands.h"
#include "symbol_table.h"
#include "util.h"

int CODE_IMG[MAX_DATA_SIZE];

cmd cmd_table[CMD_SUM] = {
    {"mov", mov, 2},
    {"cmp", cmp, 2},
    {"add", add, 2},
    {"sub", sub, 2},
    {"not", not, 1},
    {"clr", clr, 1},
    {"lea", lea, 2},
    {"inc", inc, 1},
    {"dec", dec, 1},
    {"jmp", jmp, 1},
    {"bne", bne, 1},
    {"red", red, 1},
    {"prn", prn, 1},
    {"jsr", jsr, 1},
    {"rts", rts, 0},
    {"stop", stop, 0}
};

cmd find_cmd(char* cmd_name){
    int i;
    for (i=0; i<CMD_SUM; i++){
        if (strcmp(cmd_table[i], cmd_name) == 0)
            return cmd_table[i]; /* if here then found the cmd */
    }
    return NULL; /* return null if not found anything */
}

void add_machine_word(machine_word current_word, int IC){
    int word_location = IC + START_ADDRESS;
    int built_word = current_word.source; /* slot 11-9 */
    built_word <<= 4 + current_word.op_code;  /* slot 8-5 */
    built_word <<= 3 + current_word.dest; /* slot 4-2 */
    built_word <<= 2; /* slot 1-0 since encoding is always 00  */
    CODE_IMG[word_location] = built_word ;
}

/* since register adressing can be both source or dest need bool flag to know which bits to flag */
bool add_extra_word_single_param(parameter param, bool is_source, int IC){
    int word_location = IC + START_ADDRESS;
    char* end_ptr;
    int new_num;
    symbol_data* symbol;
    if (param.address == register_addr){
        new_num = param.param_name[2] - '0'; /* get register number */
        /* shifting 7 if source since source is 11-7 and 2 for dest since dest is 6-2 */
        new_num = is_source ? new_num<<7 : new_num<<2;
        CODE_IMG[word_location] = new_num;
    } else if (param.address == immediate) {
        new_num = strtol(param.param_name,&end_ptr,10); /* convert given str to base 10 long */
        if (*end_ptr != '\0'){
            fprintf(stderr, "Line %d with variable %s,unable to convert,got %d after convertion",current_line, param.param_name, new_num);
            return false;
        }
        if (new_num > MAX_NUMBER|| new_num < MIN_NUMBER){ /* out of numbers range */
            fprintf(stderr, "Line %d with number %d,out of numbers range,max is %d and min is %d",current_line,new_num,MAX_NUMBER,MIN_NUMBER);
            return false;
        }
        new_num<<=2; /* make room for 00 */
        CODE_IMG[word_location] = new_num;
    } else if (param.address == direct) {
        ///* TODO: need to handle in second pass */
        if((symbol = find_symbol(param.param_name)) == NULL){
            fprintf(stderr, "Line %d unable to find label %s",current_line,param.param_name);
            return false;
        }
        new_num = symbol->symbol.value;
        if (symbol->symbol.attribute == SYMBOL_EXTERN){
            new_num<<2 + external; /* if external then add const for 01 bits in 1-0 location */
            ///* TODO: complete write_external_file function */
            write_external_file(symbol->symbol.name, word_location);
        } else {
            /* if not external then label needs reloaction so add the const for 10 bits in 1-0 location */
            new_num<<2 + realocatable;
        }
    }
    return true;
}

/* for when both the operans are registers and share the word */
void add_extra_word_double_param(char* source, char* dest, int IC){
    int word_location = IC + START_ADDRESS;
    /* since checked before that register name are legal, get the register number */
    int new_num = source[2] - '0'; /* setting the first 5 digits 11-7 */
    int reg_2 = dest[2] - '0';
    new_num <<= 5 + reg_2; /* shifting 5 for the next 5 digits 6-1 */
    new_num <<= 2; /* last shift of 2 to set all in place, always 00 in digits 1-0 */
    CODE_IMG[word_location] = new_num;
}

void find_parameters(parameter first_param, parameter second_param){
    char* token;
    char* end_ptr;
    int new_num;
    bool has_comma = true;
    /* meaning no text after cmd so no parameters for addressing */
    memset(first_param.param_name, 0, strlen(first_param.param_name));
    memset(second_param.param_name, 0, strlen(second_param.param_name));
    first_param.address = adders_error;
    second_param.address = adders_error;
    if((token = strtok(NULL, " ")) == NULL){
        first_param.address = no_addresing;
        return;
    }
    strcpy(first_param.param_name, token);
    if (is_register(token)){
        first_param.address = register_addr;
    } else { /* could be label or immediate value */
        /* only labels start with char */
        if (isalpha(token[0])){
            if (isReservedWord(token)){
                fprintf(stderr, "Line %d label reference cannot be a reserved word",current_line);
                return;
            }
            first_param.address = direct;
        } else { /* starts with number so only immediate */
            new_num = strtol(token,&end_ptr,10); /* convert given str to base 10 long */
            if (*end_ptr != '\0'){
                fprintf(stderr, "Line %d with variable %s,unable to convert,got %d after convertion",current_line, name, new_num);
                return;
            }
            if (new_num > MAX_NUMBER|| new_num < MIN_NUMBER){ /* out of numbers range */
                fprintf(stderr, "Line %d with number %d,out of numbers range,max is %d and min is %d",current_line,new_num,MAX_NUMBER,MIN_NUMBER);
                return;
            }
            first_param.address = direct;
        }
    }
    /* after first param should be a comma */
    if((token = strtok(NULL, " ")) != ','){
        second_param.address = no_addresing;
        has_comma = false;
    }
    /* same checks as first param but for second param */
    if((token = strtok(NULL, " ")) == NULL){
        second_param.address = no_addresing;
        return;
    }
    if (!has_comma){
        fprintf(stderr, "Line %d missing comma after first parameter",current_line);
        return;
    }
    strcpy(second_param.param_name, token);
    if (is_register(token)){
        second_param.address = register_addr;
    } else { /* could be label or immediate value */
        /* only labels start with char */
        if (isalpha(token[0])){
            if (isReservedWord(token)){
                fprintf(stderr, "Line %d label reference cannot be a reserved word",current_line);
                return;
            }
            second_param.address = direct;
        } else { /* starts with number so only immediate */
            new_num = strtol(token,&end_ptr,10); /* convert given str to base 10 long */
            if (*end_ptr != '\0'){
                fprintf(stderr, "Line %d with variable %s,unable to convert,got %d after convertion",current_line, name, new_num);
                return;
            }
            if (new_num > MAX_NUMBER|| new_num < MIN_NUMBER){ /* out of numbers range */
                fprintf(stderr, "Line %d with number %d,out of numbers range,max is %d and min is %d",current_line,new_num,MAX_NUMBER,MIN_NUMBER);
                return;
            }
            second_param.address = direct;
        }
    }
    /* check for extreneous text after second parameter */
    if((token = strtok(NULL, " ")) != NULL){
        fprintf(stderr,"Line %d extreneous text after second parameter",current_line);
    }
}