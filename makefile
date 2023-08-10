CC = gcc
CFLAGS = -Wall -ansi -pedantic -std=c11 -ggdb3
SRC_FILES = mcroUnfold.c util.c macro_table.c
HEADER_FILES = util.h macro_table.h
OUTPUT = myProg

$(OUTPUT): $(SRC_FILES) $(HEADER_FILES)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC_FILES)

clean:
	rm -f $(OUTPUT)

