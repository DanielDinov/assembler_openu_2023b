#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* define bool type */
typedef enum bool_t {
    false = 0,
    true
} bool;

/* define op_code enum */
typedef enum op_code_t {
    op_code_error = -1,
    mov = 0,
    cmp,
    add,
    sub,
    not,
    clr,
    lea,
    inc,
    dec,
    jmp,
    bne,
    red,
    prn,
    jsr,
    rts,
    stop,
} op_code_type;

/* define are enum */
typedef enum are_t {
    encoding_error = -1,
    absolute = 0,
    external = 1,
    realocatable = 2
} encoding_type;

/* define addressing type enum */
typedef enum adressing_t {
    adders_error = -1,
    immediate = 1,
    direct = 3,
    register = 5
} adressing_type;

/* machine word struct with all required parameters */
typedef struct macine_word_t {
    encoding_type encoding;
    adressing_type dest;
    op_code_type op_code;
    adressing_type source;
} machine_word;

/* command types, to initialize command array in order to retrive data */
typedef struct command_t {
    char command_name[5];
    op_code_type op_code;
    int num_of_operands;
} cmd;


#endif