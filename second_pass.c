#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "commands.h"
#include "data_handler.h"
#include "symbol_table.h"
#include "util.h"

/* TODO reset to null pointers right after use */
/* second_pass gets an .am file and access to symbol table to update and finish the code and data images */

bool secondPass(FILE* file, char* fileName)
{
	char line[MAX_LINE_LEN + 2];
	char* token = NULL;
	bool success_flag = true, update_entry = false, make_entry = false;
	int ic = 0, current_line = 0;
	symbol_data* symbol = NULL;
	parameter first_param, second_param;
    cmd* current_cmd;

	while (fgets(line, MAX_LINE_LEN + 2, file) != NULL){
        current_line++;
		token = strtok(line, delims);

        if (token[strlen(token) - 1] == ':') /* has label */
            token = strtok(NULL, delims);

        if (token[0] == '.'){
            if (strcmp(token, ".entry") == 0){
                token = strtok(NULL, delims);
                if((symbol = find_symbol(token)) == NULL){
                    fprintf(stderr, "Line %d unable to find label %s",current_line,token);
                    success_flag = false;
                    continue;
                } else {
                    if (symbol->symbol.attribute == SYMBOL_EXTERN){
                        fprintf(stderr, "Line %d external label cannot be used in .entry",current_line);
                        success_flag = false;
                    } else {
                        write_entry_file(symbol->symbol, fileName);
                    }
                } 
            }
        } else {
            /* commands here */
            if((current_cmd = find_cmd(token)) == NULL){
                fprintf(stderr, "Line %d bad command,unable to process %d",current_line,token);
                success_flag = false;
                continue;
            }
            find_parameters(first_param, second_param);

            ic++; /* since already added memory word in first pass */

            switch (current_cmd -> num_of_operands){

                case 0:
                    if (first_param.address != no_addresing){
                        fprintf(stderr, "Line %d cmd %s shouldnt receive parameters",current_line,current_cmd->command_name);
                        success_flag = false;
                    }
                    break;
                
                case 1:
                    if (first_param.address == no_addresing || second_param.address != no_addresing){
                        fprintf(stderr, "Line %d cmd %s should receive 1 parameter",current_line,current_cmd->command_name);
                        success_flag = false;
                    }
                    /* handling direct access since have all the data now */
                    if (first_param.address == direct)
                        if (!add_extra_word_single_param(first_param,false,ic))
                            success_flag = false;
                        ic++;
                    break;

                case 2:
                    if (first_param.address == no_addresing || second_param.address == no_addresing){
                        fprintf(stderr, "Line %d cmd %s should receive 2 parameter",current_line,current_cmd->command_name);
                        success_flag = false;
                    }
                    /* when both addressing types are register they share a single word */
                    if (first_param.address == register_addr && second_param.address == register_addr){
                        ic++;
                        continue;

                    } else { /* meaning 1 of the addressing type is not register addressing */
                        if (first_param.address != adders_error && second_param.address != adders_error){
                            if (first_param.address == direct)
                                if (!add_extra_word_single_param(first_param,false,ic))
                                    success_flag = false;
                            ic++;
                            if (second_param.address == direct)
                                if (!add_extra_word_single_param(first_param,false,ic))
                                    success_flag = false;
                            ic++;
                        }
                    }
                    break;
                
                default:
                    fprintf(stderr, "Line %d bad command,unable to process %d",current_line,token);
                    success_flag = false;
                    break;
            }
        }
	}

    return success_flag;
}