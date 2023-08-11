#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* since a since word is 12 bits this is the max size (2^12-1) to (2^12-1)-1 */
#define MAX_NUMBER 2047
#define MIN_NUMBER -2048

#define SYMBOL_MAX_NAME_SIZE 31
#define MAX_LINE_LEN 80
#define MAX_DATA_SIZE 1024

#define INITIAL_ADDRESS 100

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
typedef enum addressing_t {
    adders_error = -1,
    no_addresing = 0,
    immediate = 1,
    direct = 3,
    register_addr = 5
} addressing_type;

/* machine word struct with all required parameters */
typedef struct macine_word_t {
    encoding_type encoding;
    addressing_type dest;
    op_code_type op_code;
    addressing_type source;
} machine_word;

/* command types, to initialize command array in order to retrive data */
typedef struct command_t {
    char command_name[5];
    op_code_type op_code;
    int num_of_operands;
} cmd;

typedef struct parameter_t {
    char param_name[SYMBOL_MAX_NAME_SIZE + 1];
    addressing_type address;
} parameter;

extern int current_line;

char* am_extension = ".am";
char* as_extension = ".as";

#endif