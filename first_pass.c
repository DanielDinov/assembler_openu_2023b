#include "commands.h"
#include "data_handler.h"
#include "symbol_table.h"
#include "util.h"

bool first_pass(char* file_name){
    int ic = 0,dc = 0, current_line = 0,dc_incerement;
    char line[MAX_LINE_LEN + 1] = "", tmp_line_for_display[MAX_LINE_LEN + 1];
    char* token;
    bool sucess_flag = true, has_label;
    FILE* working_file;
    char symbol_name[SYMBOL_MAX_NAME_SIZE+1];
    parameter first_param, second_param;
    cmd current_cmd;
    machine_word current_machine_word;

    if ((working_file = fopen(file_name, "r")) == NULL){
        printf("Failed to open file %s",file_name);
        return false; /* nothing to continue with */
    }
    while (fgets(line, MAX_LINE_LEN+1, working_file) != NULL){
        ///* TODO: reset variables */

        has_label = false;
        strcpy(tmp_line_for_display,line);
        
        current_line++;
        format_line(line);

        if (line[0] == ';' | lineToIgnore(line))
            continue;
        
        if ((token = strtok(line, " ")) == NULL){
            fprintf(stderr, "Line %d failed to handle line %s",current_line,tmp_line_for_display);
            sucess_flag = false;
        }

        /* checking for symbol */
        if (token[strlen(token) -1] == ':'){
            /* not checking for len since was defined as 31 in start of project */
            if (!isalpha(token[0])){
                fprintf(stderr, "Line %d label cannot start with number",current_line);
                sucess_flag = false;
            }

            /* copying token into variable for each of access later on */
            strncpy(symbol_name, token, strlen(token) -1);
            symbol_name[strlen(token) -1] = '\0'; /* adding null terminator to signify end of string */
            has_label = true;

            /* checking for reserved word and empty declarations */
            if (isReservedWord(symbol_name)){
                fprintf(stderr, "Line %d label cannot be a reserved word",current_line);
                sucess_flag = false;
            }
            if ((token = strtok(NULL, " ")) == NULL){
                fprintf(stderr, "Line %d cannot have empty label declaration",current_line);
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
                    fprintf(stderr,"Line %d no parameters after .data line",current_line);
                    sucess_flag = false;
                }
                while (token){
                    if (!add_data_num(token,dc)){
                        fprintf(stderr,"Line %d failed to add numbers to data image",current_line);
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
                    fprintf(stderr, "Line %d no parameters after .string line",current_line);
                    sucess_flag = false;
                }
                dc_incerement = add_data_string(token,dc);
                if (dc_incerement == 0 || dc_incerement == -1){
                    fprintf(stderr, "Line %d failed to add string to data image",current_line);
                    sucess_flag = false;
                } else { dc+=dc_incerement }
            } else if (strcmp(token, ".extern") == 0){
                if((token = strtok(NULL, " ")) == NULL){
                    fprintf(stderr, "Line %d no parameters after .extern line",current_line);
                    sucess_flag = false;
                }
                if (!add_symbol_to_list(token,dc,SYMBOL_EXTERN))
                    sucess_flag = false;
            } else if (strcmp(token, ".extry") == 0){
                /* here so it wont fall in the next else cluase but will be handled in 2nd pass */
            } else {
                fprintf(stderr, "Line %d unknown request",current_line);
                sucess_flag = false;
                /* clears token to not fall in the extraneous text clause */
                while(token)
                    token = strtok(NULL," ")
            }
            if ((token = strok(NULL, " ")) != NULL){
                fprintf(stderr, "Line %d extraneous text after request",current_line);
                sucess_flag = false;
            }
        }
        /* if here meaning command type line */
        else {
            ///* TODO: handle command type line */
            if (has_label){
                if (!add_symbol_to_list(symbol_name,dc,SYMBOL_DATA))
                    sucess_flag = false;
            }
            if((current_cmd = find_cmd(token)) == NULL){
                fprintf(stderr, "Line %d bad command,unable to process %d",current_line,token);
                sucess_flag = false;
                return false; /* return to avoid NULL access violation */
            }
            find_parameters(first_param, second_param);

            current_machine_word.op_code = current_cmd.op_code;
            current_machine_word.source = first_param.address;
            current_machine_word.dest = second_param.address;

            add_machine_word(current_machine_word,ic);
            ic++;

            switch (current_cmd.num_of_operands){
            case 0:
                if (first_param.address != no_addresing){
                    fprintf(stderr, "Line %d cmd %s shouldnt receive parameters",current_line,current_cmd.command_name);
                    sucess_flag = false;
                }
                break;
            
            case 1:
                if (first_param.address == no_addresing || second_param != no_addresing){
                    fprintf(stderr, "Line %d cmd %s should receive 1 parameter",current_line,current_cmd.command_name);
                    sucess_flag = false;
                }
                /* direct addressing will be handled in second pass since not enough data currently */
                if (first_param.address == register_addr || first_param.address == immediate)
                    if (!add_extra_word_single_param(first_param,false,ic))
                        sucess_flag = false;
                ic++
                break;
            
            case 2:
                if (first_param.address == no_addresing || second_param == no_addresing){
                    fprintf(stderr, "Line %d cmd %s should receive 2 parameter",current_line,current_cmd.command_name);
                    sucess_flag = false;
                }
                /* when both addressing types are register they share a single word */
                if (first_param.address == register_addr && second_param.address == register_addr){
                    add_extra_word_double_param(first_param.param_name, second_param.param_name, ic);
                    ic++;
                } else { /* meaning 1 of the addressing type is not register addressing */
                    if (first_param.address != adders_error && second_param.address != adders_error){
                        if (first_param.address == register_addr || first_param.address == immediate)
                            if (!add_extra_word_single_param(first_param,true,ic))
                                sucess_flag = false;
                        ic++;
                        if (second_param.address == register_addr || second_param.address == immediate)
                            if (!add_extra_word_single_param(second_param,false,ic))
                                sucess_flag = false;
                        ic++;
                    }
                }
                break;
            
            default:
                fprintf(stderr, "Line %d bad command,unable to process %d",current_line,token);
                sucess_flag = false;
                break;
            }
        }

    }
    return sucess_flag;
}