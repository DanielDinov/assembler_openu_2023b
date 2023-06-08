#include <stdio.h>

/* reads line from stdio and formats in a tokenizable format, sets the value in the given pointer */
void read_line(char*);

void read_line(char* line){
    char c;
    int in_word = 0;
    int buffer_index = 0;
    /* build the string from input line by line or untill eof while skipping spaces and tabs */
    while((c=getchar()) != '\n' && c != EOF){
        /* skip over space and tab delimiters but with a flag make sure i include atleast one */
        if (c == ' ' || c == '\t'){
            if (in_word){
                line[buffer_index++] = ' ';
                in_word = 0;
            }
            continue;
        }
        /* space the built string correctly */
        if (c == ','){
            line[buffer_index++] = ' ';
            line[buffer_index++] = ',';
            continue;
        }
        line[buffer_index++] = c;
        in_word = 1;
    }
    /* end string with agreed upon finisher */
    line[buffer_index] = '\0';
}