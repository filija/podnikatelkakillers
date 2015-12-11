CFLAGS=-std=c99 -Wall -pedantic -g
BIN=project
CC=gcc
RM=rm -f

ALL: str.o scanner.o parser.o tabulka_symbolu.o instruction_list.o interpret.o main.o
	$(CC) $(CFLAGS) -o $(BIN) str.o scanner.o parser.o tabulka_symbolu.o instruction_list.o interpret.o main.o 

clean:
	$(RM) *.o $(BIN)
