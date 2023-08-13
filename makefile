CC = gcc

CFLAGS = -Wall -ansi -pedantic -std=c11 -ggdb3

SRC_FILES = assembler.c util.c macro_unfold.c macro_table.c first_pass.c second_pass.c commands.c data_handler.c

HEADER_FILES = globals.h util.h macro_unfold.h macro_table.h first_pass.h second_pass.h commands.h data_handler.h

OUTPUT = myProg



$(OUTPUT): $(SRC_FILES) $(HEADER_FILES)

	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC_FILES)



clean:

	rm -f $(OUTPUT)

