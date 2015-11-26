CFLAGS=-std=c99 -pedantic -Wall -g
BIN=project
CC=gcc
RM=rm -f

all: ial.o parser.o main.o syntax.o scanner.o project

	#SCANNER
scanner.o: scanner.c
	$(CC) $(CFLAGS) -c -o scanner.o scanner.c

	#STR
str.o: str.c
	$(CC) $(CFLAGS) -c -o str.o str.c
	#IAL
ial.o: ial.c ial.h
	$(CC) $(CFLAGS) -c -o ial.o ial.c

	#SYNTAX
syntax.o: syntax.c
	$(CC) $(CFLAGS) -c -o syntax.o syntax.c

	#PARSER
parser.o: parser.c scanner.c syntax.c
	$(CC) $(CFLAGS) -c -o parser.o parser.c



	