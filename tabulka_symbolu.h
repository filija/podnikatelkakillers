#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "define.h"

// #define TRUE 1
// #define FALSE 0
typedef struct sym tSymbol;
typedef struct sym *tSymbolPtr;
typedef struct SymbolTable tSymbolTable;
typedef struct SymbolTable *uk_uzel;

typedef union
{
    int i;
    float f;
    char *s;
} uObsah;


struct sym   // symbol
{
    char *symbol;       // nazev promenne
    int typ;    // datovy typ symbolu
    int verze;         // 1 funkce 0 promenna
    uObsah value;        // obsah int, float, string
    char *par_typy; // typy parametru void f(string, bool, bool) >> vsbb
    int defined; // 1 ano, 0 ne
    uk_uzel tabulka; // ukazatel na tabulku - glob ->lok
    // ukazatel na label
};

struct SymbolTable
{
    char *symbol;
    tSymbolPtr data;
    struct SymbolTable *LPtr;
    struct SymbolTable *RPtr;
};


void inicializuj_tabulku (uk_uzel*);
tSymbolPtr najdi_v_tabulce  (uk_uzel, char*);
int vloz_do_tabulky (uk_uzel*, char*, tSymbolPtr);
void znic_tabulku(uk_uzel);
//void check(uk_uzel);
int copy_item(tSymbolPtr*, tSymbolPtr);
