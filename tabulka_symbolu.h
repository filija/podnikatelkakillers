#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// #define TRUE 1
// #define FALSE 0

typedef union
{
    int i;
    float f;
    char *s;
} uObsah;


typedef struct sym   // symbol
{
    char *symbol;       // nazev promenne
    int typ;    // datovy typ symbolu
    int verze;         // 1 funkce 0 promenna
    uObsah value;        // obsah int, float, string
    char *par_typy; // typy parametru void f(string, bool, bool) >> vsbb
    int defined; // 1 ano, 0 ne
} tSymbol, *tSymbolPtr;

typedef struct SymbolTable
{
    char *symbol;
    tSymbol data;
    struct SymbolTable *LPtr;
    struct SymbolTable *RPtr;
} tSymbolTable, *uk_uzel;


void inicializuj_tabulku (uk_uzel);
tSymbolPtr najdi_v_tabulce  (uk_uzel, char*);
int vloz_do_tabulky (uk_uzel, char*, tSymbol);
void znic_tabulku(uk_uzel);
void check(uk_uzel);
