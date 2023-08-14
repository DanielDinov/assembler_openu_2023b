# Compilation macros
 CC = gcc
 CFLAGS = -Wall -ansi -pedantic -std=c11 -ggdb3 # Flags
 GLOBAL_DEPS = globals.h # Dependencies for everything
 EXE_DEPS = assembler.o util.o macro_table.o macro_unfold.o commands.o data_handler.o symbol_table.o first_pass.o second_pass.o # Deps for exe
 
 ## Executable
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@
	
assembler.o:  assembler.c $(GLOBAL_DEPS)
	$(CC) -c assembler.c $(CFLAGS) -o $@

macro_unfold.o: macro_unfold.c macro_unfold.h $(GLOBAL_DEPS)
	$(CC) -c macro_unfold.c $(CFLAGS) -o $@

first_pass.o: first_pass.c first_pass.h $(GLOBAL_DEPS)
	$(CC) -c first_pass.c $(CFLAGS) -o $@

second_pass.o: second_pass.c second_pass.h $(GLOBAL_DEPS)
	$(CC) -c second_pass.c $(CFLAGS) -o $@

commands.o: commands.c commands.h $(GLOBAL_DEPS)
	$(CC) -c commands.c $(CFLAGS) -o $@

data_handler.o: data_handler.c data_handler.h $(GLOBAL_DEPS)
	$(CC) -c data_handler.c $(CFLAGS) -o $@

macro_table.o: macro_table.c macro_table.h $(GLOBAL_DEPS)
	$(CC) -c macro_table.c $(CFLAGS) -o $@

util.o: util.c util.h $(GLOBAL_DEPS)
	$(CC) -c util.c $(CFLAGS) -o $@

symbol_table.o: symbol_table.c symbol_table.h $(GLOBAL_DEPS)
	$(CC) -c symbol_table.c $(CFLAGS) -o $@
