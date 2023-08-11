#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "commands.h"
#include "data_handler.h"
#include "symbol_table.h"
#include "util.h"

/* second_pass gets an .am file and access to symbol table to update and finish the code and data images */

bool secondPass(FILE* file, char* fileName)
{
	char line[MAX_LINE_LEN + 2];
	char* token = NULL;
	bool success_flag = true, update_entry = false;
	int ic = 0, current_line = 0;
	symbol_data* symbol = NULL;
	parameter first_param, second_param;
    cmd current_cmd;

	while (fgets(line, MAX_LINE_LEN + 2, file) != NULL)
	{
		token = strtok(line, delims);
		while (token != NULL)
		{
			if(update_entry)
			{
				/*find symbol in table and update*/
				/*add it to entry table*/
				/*TODO flags for if found entry create .ent*/
				update_entry = false;
				strtok(NULL, delims);
			}

			if (token[strlen(token) - 1] == ':')
				strtok(NULL, delims);

			if (strcmp(token, ".extern") == 0 || strcmp(token, ".string") == 0 || strcmp(token, ".data") == 0)
				continue;

			if (strcmp(token, ".entry") == 0)
			{
				update_entry = true;
				strtok(NULL, delims);
			}

            if((current_cmd = find_cmd(token)) == NULL){
                fprintf(stderr, "Line %d bad command,unable to process %d",current_line,token);
                success_flag = false;
                return false; /* return to avoid NULL access violation */
            }
            find_parameters(first_param, second_param);

            ic++;

            switch (current_cmd.num_of_operands)
			{
            case 0:
                if (first_param.address != no_addresing)
				{
                    fprintf(stderr, "Line %d cmd %s shouldnt receive parameters",current_line,current_cmd.command_name);
                    success_flag = false;
                }
                break;
            
            case 1:
                if (first_param.address == no_addresing || second_param.address != no_addresing)
				{
                    fprintf(stderr, "Line %d cmd %s should receive 1 parameter",current_line,current_cmd.command_name);
                    success_flag = false;
                }
                /* direct addressing will be handled in second pass since not enough data currently */
                if (first_param.address == register_addr || first_param.address == immediate)
                	ic++;
                break;
            
            case 2:
                if (first_param.address == no_addresing || second_param.address == no_addresing)
				{
                    fprintf(stderr, "Line %d cmd %s should receive 2 parameter",current_line,current_cmd.command_name);
                    success_flag = false;
                }
                /* when both addressing types are register they share a single word */
                if (first_param.address == register_addr && second_param.address == register_addr)
				{
                    ic++;
                } else { /* meaning 1 of the addressing type is not register addressing */
                    if (first_param.address != adders_error && second_param.address != adders_error){
                        if (first_param.address == register_addr || first_param.address == immediate)
                        ic++;
                        if (second_param.address == register_addr || second_param.address == immediate)
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

		if ((symbol = find_symbol(token)) != NULL)
			add_machine_word_symbol(symbol->symbol.value);

		if (isDirective(token) != 0)
		{

		}
		
	}