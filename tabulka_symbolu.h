#ifndef TABULKA_SYMBOLU_H
#define TABULKA_SYMBOLU_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "define.h"
#include "str.h"
#include "instruction_list.h"

// #define TRUE 1
// #define FALSE 0
typedef struct sym tSymbol;
typedef struct sym *tSymbolPtr;
typedef struct SymbolTable tSymbolTable;
typedef struct SymbolTable *uk_uzel;

typedef union
{
    int i;
    double d;
    char *s;
} uObsah;

typedef struct sym_param
{
    char *name;
    int typ;
    struct sym_param *next;
} param;

struct sym   // symbol
{
    char *symbol;       // nazev promenne
    int typ;    // datovy typ symbolu 0 = nedef, 1 = int, 2 = double, 3 = string
    int verze;         // 1 funkce 0 promenna
    uObsah value;        // obsah int, double, string
    param *parametry; // typy a jmena parametru
    int defined; // 1 ano, 0 ne
    uk_uzel tabulka; // ukazatel na tabulku - glob ->lok
    tInstr *label;
};

struct SymbolTable
{
    char *symbol;
    tSymbolPtr data;
    struct SymbolTable *LPtr;
    struct SymbolTable *RPtr;
};

typedef struct ZasobnikTabulek
{
    uk_uzel tabulka;
    int zarazka;    //1 nepokracovat, 0 zarazka neni
    struct ZasobnikTabulek *next;
}*tStackPtr;


void inicializuj_tabulku (uk_uzel*);
int inicializuj_data(tSymbolPtr *symbol);
tSymbolPtr najdi_v_tabulce  (uk_uzel, char*);
int vloz_do_tabulky (uk_uzel*, char*, tSymbolPtr);
void znic_tabulku(uk_uzel);
//void check(uk_uzel);
int copy_item(tSymbolPtr*, tSymbolPtr);
int vloz_do_parametru(int typ, char* nazev, param **parametry_v);
int zkontroluj_parametry(param *parametr, tSymbolPtr data);
int push_tstack(tStackPtr *stack, uk_uzel tabulka, int zarazka);
int pop_tstack(tStackPtr *stack);
tSymbolPtr find_tstack(tStackPtr stack, char* name);
tSymbolPtr out_find_tstack(tStackPtr stack, char* name);
int insert_tstack(tStackPtr stack, char *K, tSymbolPtr data);
int check_defined(uk_uzel glob_table);

#endif /* TABULKA_SYMBOLU_H */