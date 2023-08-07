#include "commands.h"
#include "data_handler.h"
#include  "symbol_table.h"

bool first_pass(char* file_name){
    /* TODO: handle starting line at 100 */
    int ic = 0,dc = 0, current_line = 0,dc_incerement;
    char line[MAX_LINE_LEN + 1] = "", tmp_line_for_display[MAX_LINE_LEN + 1];
    char* token;
    bool sucess_flag = true, has_label;
    FILE* working_file;
    char symbol_name[SYMBOL_MAX_NAME_SIZE+1];

    if ((working_file = fopen(file_name, "r")) == NULL){
        printf("Failed to open file %s",file_name);
        return false; /* nothing to continue with */
    }
    while (fgets(line, MAX_LINE_LEN+1, working_file) != NULL){
        /* TODO: reset variables */

        has_label = false;
        strcpy(tmp_line_for_display,line);
        
        current_line++;
        /* TODO: handle formatting of line */
        format_line(line);
        if (line[0] == ';' | is_line_empty(line))
            continue;
        
        if ((token = strtok(line, " ")) == NULL){
            fprintf("Line %d failed to handle line %s",current_line,tmp_line_for_display);
            sucess_flag = false;
        }

        /* checking for symbol */
        if (token[strlen(token) -1] == ':'){
            /* not checking for len since was defined as 31 in start of project */
            if (!isalpha(token[0])){
                fprintf("Line %d label cannot start with number",current_line);
                sucess_flag = false;
            }
            /* TODO: check for saves words */

            strncpy(symbol_name, token, SYMBOL_MAX_NAME_SIZE+1);
            has_label = true;
            if ((token = strtok(NULL, " ")) == NULL){
                fprintf("Line %d cannot have empty label declaration",current_line);
                sucess_flag = false;
            }
        }
        
        /* meaning .data .string .entry .extern */
        if (token != NULL && token[0] == '.'){
            if (strcmp(token, ".data") == 0){
                if (has_label){
                    if (!add_symbol_to_list(symbol_name,dc,SYMBOL_DATA))
                        sucess_flag = false;
                }
                if((token = strtok(NULL, " ")) == NULL){
                    fprintf("Line %d no parameters after .data line",current_line);
                    sucess_flag = false;
                }
                while (token){
                    if (!add_data_num(token,dc)){
                        fprintf("Line %d failed to add numbers to data image",current_line);
                        sucess_flag = false;
                        break;
                    }
                    /* increment dc if sucessfully added data */
                    dc++;
                    token = strtok(NULL, ",");
                }
            } else if (strcmp(token, ".string") == 0){
                if (has_label){
                    if (!add_symbol_to_list(symbol_name,dc,SYMBOL_DATA))
                        sucess_flag = false;
                }
                if((token = strtok(NULL, " ")) == NULL){
                    fprintf("Line %d no parameters after .string line",current_line);
                    sucess_flag = false;
                }
                dc_incerement = add_data_string(token,dc);
                if (dc_incerement == 0 || dc_incerement == -1){
                    fprintf("Line %d failed to add string to data image",current_line);
                    sucess_flag = false;
                } else { dc+=dc_incerement }
            } else if (strcmp(token, ".extern") == 0){
                if((token = strtok(NULL, " ")) == NULL){
                    fprintf("Line %d no parameters after .extern line",current_line);
                    sucess_flag = false;
                }
                if (!add_symbol_to_list(token,dc,SYMBOL_DATA))
                    sucess_flag = false;
            } else if (strcmp(token, ".extry") == 0){
                /* here so it wont fall in the next else cluase but will be handled in 2nd pass */
            } else {
                fprintf("Line %d unknown request",current_line);
                sucess_flag = false;
                /* clears token to not fall in the extraneous text clause */
                while(token)
                    token = strtok(NULL," ")
            }
            if ((token = strok(NULL, " ")) != NULL){
                fprintf("Line %d extraneous text after request",current_line);
                sucess_flag = false;
            }
        }
        /* if here meaning command type line */
        else {
            /* TODO: handle command type line */
        }

    }
}