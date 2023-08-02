#include "commands.h"

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