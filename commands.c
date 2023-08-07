#include "commands.h"
#include  "symbol_table.h"

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
bool add_extra_word_single_param(char* name, addressing_type address, bool is_source, int IC){
    int word_location = IC + START_ADDRESS;
    char* end_ptr;
    int new_num;
    symbol_data* symbol;
    if (address == register_addr){
        new_num = name[2] - '0'; /* get register number */
        /* shifting 7 if source since source is 11-7 and 2 for dest since dest is 6-2 */
        new_num = is_source ? new_num<<7 : new_num<<2;
        CODE_IMG[word_location] = new_num;
    } else if (address == immediate) {
        new_num = strtol(name,&end_ptr,10); /* convert given str to base 10 long */
        if (*end_ptr != '\0'){
            fprintf(stderr, "Line %d with variable %s,unable to convert,got %d after convertion",current_line, name, new_num);
            return false;
        }
        if (new_num > MAX_NUMBER|| new_num < MIN_NUMBER){ /* out of numbers range */
            fprintf(stderr, "Line %d with number %d,out of numbers range,max is %d and min is %d",current_line,new_num,MAX_NUMBER,MIN_NUMBER);
            return false;
        }
        new_num<<=2; /* make room for 00 */
        CODE_IMG[word_location] = new_num;
    } else if (address == direct) {
        /* TODO: add direct address logic */
        /* TODO: need to handle in second pass */
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