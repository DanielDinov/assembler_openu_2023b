#include "commands.h"
#include "data_handler.h"
#include "symbol_table.h"
#include "util.h"

/* first pass gets a file name and reads through the .am file, brake and analyze the given assembly code.
   it creates .ext file if external exist.
   return true if no erros found, else return false*/

bool firstPass(char* file_name){
    int ic = 0,dc = 0, current_line = 0,dc_incerement;
    char line[MAX_LINE_LEN + 3] = "", tmp_line_for_display[MAX_LINE_LEN + 3];
    char *token, *file_name_extended = str_allocate_cat(file_name, am_extension);
    bool success_flag = true, has_label;
    FILE* working_file;
    char symbol_name[SYMBOL_MAX_NAME_SIZE+1];
    parameter first_param, second_param;
    cmd* current_cmd;
    machine_word current_machine_word;

    if ((working_file = fopen(file_name_extended, "r")) == NULL){
        printf("Failed to open file %s\n",file_name_extended);
        free(file_name_extended);
        return false; /* nothing to continue with */
    }
    free(file_name_extended);

    while (fgets(line, MAX_LINE_LEN+3, working_file) != NULL){
        /* reset variables block */
        tmp_line_for_display[0] = '\0';
        symbol_name[0] = '\0';
        has_label = false;
        first_param.address = -1;
        first_param.param_name[0] = '\0';
        second_param.address = -1;
        second_param.param_name[0] = '\0';
        current_cmd = NULL;
        current_machine_word.dest = -1;
        current_machine_word.encoding = -1;
        current_machine_word.op_code = -1;
        current_machine_word.source = -1;

        strcpy(tmp_line_for_display,line);
        
        current_line++;
        format_line(line);
        
        if ((token = strtok(line, delims)) == NULL){
            fprintf(stderr, "ERROR: Line %d failed to handle line %s\n",current_line,tmp_line_for_display);
            success_flag = false;
        }
        /* checking for symbol */
        if (token[strlen(token) -1] == ':'){
            /* not checking for len since was defined as 31 in start of project */
            if (!isalpha(token[0])){
                fprintf(stderr, "ERROR in %s: Line %d label cannot start with number\n", file_name, current_line);
                success_flag = false;
            }

            /* copying token into variable for each of access later on */
            strncpy(symbol_name, token, strlen(token) -1);
            symbol_name[strlen(token) -1] = '\0'; /* adding null terminator to signify end of string */
            has_label = true;

            /* checking for reserved word and empty declarations */
            if (isReservedWord(symbol_name)){
                fprintf(stderr, "ERROR in %s: Line %d label cannot be a reserved word\n", file_name, current_line);
                success_flag = false;
            }
            if ((token = strtok(NULL, delims)) == NULL){
                fprintf(stderr, "ERROR in %s: Line %d cannot have empty label declaration\n",file_name , current_line);
                success_flag = false;
            }
        }
        
        
        /* meaning .data .string .entry .extern */
        if (token != NULL && token[0] == '.'){
            if (strcmp(token, ".data") == 0){
                if (has_label){
                    if (!add_symbol_to_list(symbol_name,dc+ic+START_ADDRESS,SYMBOL_DATA)){
                    printf("data label fault\n");
                    success_flag = false;
                }
                }
                if((token = strtok(NULL, delims)) == NULL){
                    fprintf(stderr,"ERROR in %s: Line %d no parameters after .data line\n", file_name, current_line);
                    success_flag = false;
                }
                while (token){
                    if (!add_data_num(token,dc)){
                        fprintf(stderr,"ERROR in %s: Line %d failed to add numbers to data image\n", file_name, current_line);
                        success_flag = false;
                        break;
                    }
                    /* increment dc if sucessfully added data */
                    dc++;
                    token = strtok(NULL, " , ");
                }
            } else if (strcmp(token, ".string") == 0){
                if (has_label){
                    if (!add_symbol_to_list(symbol_name,dc+ic+START_ADDRESS,SYMBOL_DATA)){
                    printf("ERROR: string label fault\n");
                    success_flag = false;
                }
                }
                if((token = strtok(NULL, "\n")) == NULL){
                    fprintf(stderr, "ERROR: Line %d no parameters after .string line\n",current_line);
                    success_flag = false;
                }
                dc_incerement = add_data_string(token,dc);
                if (dc_incerement == 0 || dc_incerement == -1){
                    fprintf(stderr, "ERROR: Line %d failed to add string to data image\n",current_line);
                    success_flag = false;
                } else { dc+=dc_incerement; }
            } else if (strcmp(token, ".extern") == 0){
                if((token = strtok(NULL, delims)) == NULL){
                    fprintf(stderr, "Line %d no parameters after .extern line\n",current_line);
                    success_flag = false;
                }
                if (!add_symbol_to_list(token,0,SYMBOL_EXTERN)){
                    printf("extern label fault\n");
                    success_flag = false;
                }
            } else if (strcmp(token, ".entry") == 0){
                while(token)
                    token = strtok(NULL,delims);
                /* here so it wont fall in the next else cluase but will be handled in 2nd pass */
            } else {
                fprintf(stderr, "Line %d unknown request\n",current_line);
                success_flag = false;
                /* clears token to not fall in the extraneous text clause */
                while(token)
                    token = strtok(NULL,delims);
            }
            if ((token = strtok(NULL, delims)) != NULL){
                fprintf(stderr, "Line %d extraneous text after request\n",current_line);
                success_flag = false;
            }
        }
        /* if here meaning command type line */
        else {
            /* TODO: handle command type line */
            if (has_label){
                if (!add_symbol_to_list(symbol_name,ic+START_ADDRESS,SYMBOL_DATA)){
                    printf("cmd label fault\n");
                    success_flag = false;
                }
            }

            if((current_cmd = find_cmd(token)) == NULL){
                fprintf(stderr, "ERROR in %s: Line %d bad command,unable to process %s\n",file_name,current_line,token);
                success_flag = false;
                continue; /* continue to avoid NULL access violation */
            }
            find_parameters(&first_param, &second_param);
            current_machine_word.op_code = current_cmd->op_code;
            if (second_param.address == no_addresing){
                current_machine_word.dest = first_param.address;
                current_machine_word.source = second_param.address;
            } else {
                current_machine_word.source = first_param.address;
                current_machine_word.dest = second_param.address;
            }

            add_machine_word(current_machine_word,ic);
            ic++;
            switch (current_cmd->num_of_operands){
            case 0:
                if (first_param.address != no_addresing){
                    fprintf(stderr, "ERROR in %s: Line %d cmd %s shouldnt receive parameters\n",file_name,current_line,current_cmd->command_name);
                    success_flag = false;
                }
                break;
            
            case 1:
                if (first_param.address == no_addresing || second_param.address != no_addresing){
                    fprintf(stderr, "ERROR in %s: Line %d cmd %s should receive 1 parameter\n",file_name,current_line,current_cmd->command_name);
                    success_flag = false;
                }
                /* direct addressing will be handled in second pass since not enough data currently */
                if (first_param.address == register_addr || first_param.address == immediate)
                    if (!add_extra_word_single_param(first_param,false,ic,file_name)){
                    printf("extra word case 1 fault\n");
                    success_flag = false;
                }
                ic++;
                break;
            
            case 2:
                if (first_param.address == no_addresing || second_param.address == no_addresing){
                    fprintf(stderr, "Line %d cmd %s should receive 2 parameter\n",current_line,current_cmd->command_name);
                    success_flag = false;
                }
                /* when both addressing types are register they share a single word */
                if (first_param.address == register_addr && second_param.address == register_addr){
                    add_extra_word_double_param(first_param.param_name, second_param.param_name, ic);
                    ic++;
                } else { /* meaning 1 of the addressing type is not register addressing */
                    if (first_param.address != adders_error && second_param.address != adders_error){
                        if (first_param.address == register_addr || first_param.address == immediate)
                            if (!add_extra_word_single_param(first_param,true,ic,file_name)){
                    printf("extra word case 2 fault first param\n");
                    success_flag = false;
                }
                        ic++;
                        if (second_param.address == register_addr || second_param.address == immediate)
                            if (!add_extra_word_single_param(second_param,false,ic,file_name)){
                    printf("extra word case 1 2nd param\n");
                    success_flag = false;
                }
                        ic++;
                    }
                }
                break;
            
            default:
                fprintf(stderr, "ERROR in %s: Line %d bad command,unable to process %s\n",file_name,current_line,token);
                success_flag = false;
                break;
            }
        }

    }
    fclose(working_file);
    addIC(ic);
    addDC(dc);

    return success_flag;
}