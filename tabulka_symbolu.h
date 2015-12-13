#ifndef TABULKA_SYMBOLU_H
#define TABULKA_SYMBOLU_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "define.h"
#include "str.h"
#include "instruction_list.h"
#include "ial.h"


typedef struct ZasobnikTabulek
{
    uk_uzel tabulka;
    int zarazka;    //1 nepokracovat, 0 zarazka neni
    struct ZasobnikTabulek *next;
}*tStackPtr;

int push_tstack(tStackPtr *stack, uk_uzel tabulka, int zarazka);
int pop_tstack(tStackPtr *stack);
tSymbolPtr find_tstack(tStackPtr stack, char* name);
tSymbolPtr out_find_tstack(tStackPtr stack, char* name);
int insert_tstack(tStackPtr stack, char *K, tSymbolPtr data);
int check_defined(uk_uzel glob_table);
void znic_pracovni_tabulku(uk_uzel Koren);

#endif /* TABULKA_SYMBOLU_H */