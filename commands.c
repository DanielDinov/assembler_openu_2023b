#include "commands.h"
#include "symbol_table.h"
#include "util.h"
#include "globals.h"
#include "prints.h"

/*TODO out of array boundries check (over 1024)*/
/* commands is a data structre to store and manage the code image */

int INS_COUNTER = 0;
int CODE_IMG[MAX_DATA_SIZE] ={IMAGE_PH};

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

cmd* find_cmd(char* cmd_name){
    int i;
    for (i=0; i<CMD_SUM; i++){
        if (strcmp((cmd_table[i].command_name), cmd_name) == 0)
            return &cmd_table[i]; /* if here then found the cmd */
    }
    return NULL; /* return null if not found anything */
}

/* adding a machine word as decimal representation of the binary word */
void add_machine_word(machine_word current_word, int IC){
    int word_location = IC + START_ADDRESS;
    int built_word = current_word.source; /* slot 11-9 */
    built_word = (built_word<< 4) + current_word.op_code;  /* slot 8-5 */
    built_word = (built_word << 3) + current_word.dest; /* slot 4-2 */
    built_word = (built_word << 2); /* slot 1-0 since encoding is always 00  */
    CODE_IMG[word_location] = built_word ;
}

/* since register adressing can be both source or dest need bool flag to know which bits to flag */
bool add_extra_word_single_param(parameter param, bool is_source, int IC, char* fileName){
    int word_location = IC + START_ADDRESS;
    int new_num;
    symbol_data* symbol;
    if (param.address == register_addr){
        new_num = param.param_name[2] - '0'; /* get register number */
        /* shifting 7 if source since source is 11-7 and 2 for dest since dest is 6-2 */
        new_num = is_source ? new_num<<7 : new_num<<2;
        CODE_IMG[word_location] = new_num;
    } else if (param.address == immediate) {
        new_num = convert_to_int(param.param_name);
            if (new_num == INT_MIN){
                return false;
            }
        new_num<<=2; /* make room for 00 */
        CODE_IMG[word_location] = new_num;
    } else if (param.address == direct) {
        /* TODO: need to handle in second pass */
        if((symbol = find_symbol(param.param_name)) == NULL){
            fprintf(stderr, "ERROR in %s:Unable to find label %s add_extra_word_single_param\n", fileName, param.param_name);
            return false;
        }
        new_num = symbol->symbol.value;
        if (symbol->symbol.attribute == SYMBOL_EXTERN){
            new_num= (new_num <<2) + external; /* if external then add const for 01 bits in 1-0 location */
            write_external_file(symbol->symbol.name, word_location, fileName);
        } else {
            /* if not external then label needs reloaction so add the const for 10 bits in 1-0 location */
            new_num= (new_num <<2) + realocatable;
        }
        CODE_IMG[word_location] = new_num;
    }
    return true;
}

/* for when both the operans are registers and share the word */
void add_extra_word_double_param(char* source, char* dest, int IC){
    int word_location = IC + START_ADDRESS;
    /* since checked before that register name are legal, get the register number */
    int new_num = source[2] - '0'; /* setting the first 5 digits 11-7 */
    int reg_2 = dest[2] - '0';
    new_num = (new_num<<5) + reg_2; /* shifting 5 for the next 5 digits 6-1 */
    new_num <<= 2; /* last shift of 2 to set all in place, always 00 in digits 1-0 */
    CODE_IMG[word_location] = new_num;
}

void find_parameters(parameter* first_param, parameter* second_param){
    char* token;
    int new_num;
    bool has_comma = false;
    /* meaning no text after cmd so no parameters for addressing */
    memset(first_param->param_name, 0, strlen(first_param->param_name));
    memset(second_param->param_name, 0, strlen(second_param->param_name));
    first_param->address = adders_error;
    second_param->address = adders_error;
    if((token = strtok(NULL, delims)) == NULL){
        first_param->address = no_addresing;
        second_param->address = no_addresing;
        return;
    }
    strcpy(first_param->param_name, token);
    if (is_register(token)){
        first_param->address = register_addr;
    } else { /* could be label or immediate value */
        /* only labels start with char */
        if (isalpha(token[0])){
            if (isReservedWord(token)){
                fprintf(stderr, "label reference %s cannot be a reserved word find_parameters\n",token);
                return;
            }
            first_param->address = direct;
        } else { /* starts with number so only immediate */
            new_num = convert_to_int(token);
            if (new_num == INT_MIN)
                return;
            first_param->address = immediate;
        }
    }
    
    /* after first param should be a comma */
    if((token = strtok(NULL, delims)) == NULL || *token == ','){
        second_param->address = no_addresing;
        has_comma = true;
    }
    /* same checks as first param but for second param */
    if((token = strtok(NULL, delims)) == NULL){
        second_param->address = no_addresing;
        return;
    }
    if (!has_comma){
        fprintf(stderr, "Missing comma after first parameter %s find_parameters has comma\n",first_param->param_name);
        return;
    }
    strcpy(second_param->param_name, token);
    if (is_register(token)){
        second_param->address = register_addr;
    } else { /* could be label or immediate value */
        /* only labels start with char */
        if (isalpha(token[0])){
            if (isReservedWord(token)){
                fprintf(stderr, "Label reference %s cannot be a reserved word find_parameters\n",token);
                return;
            }
            second_param->address = direct;
        } else { /* starts with number so only immediate */
            new_num = convert_to_int(token);
            if (new_num == INT_MIN)
                return;
            second_param->address = direct;
        }
    }
    /* check for extreneous text after second parameter */
    if((token = strtok(NULL, delims)) != NULL){
        fprintf(stderr,"Extreneous text after second parameter %s\n",second_param->param_name);
    }
}

int getIC()
{
    return INS_COUNTER;
}

void addIC(int counter)
{
    INS_COUNTER = counter;
}