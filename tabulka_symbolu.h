#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "define.h"
#include "str.h"

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

typedef struct sym_param
{
	char *name;
	int typ;
	struct sym_param *next;
} param;

struct sym   // symbol
{
//    char *symbol;       // nazev promenne
    int typ;    // datovy typ symbolu 0 = nedef, 1 = int, 2 = double, 3 = string, 4 = auto
    int verze;         // 1 funkce 0 promenna
    uObsah value;        // obsah int, float, string
    param *parametry; // typy a jmena parametru
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
int inicializuj_data(tSymbolPtr *symbol);
tSymbolPtr najdi_v_tabulce  (uk_uzel, char*);
int vloz_do_tabulky (uk_uzel*, char*, tSymbolPtr);
void znic_tabulku(uk_uzel);
//void check(uk_uzel);
int copy_item(tSymbolPtr*, tSymbolPtr);
int vloz_do_parametru(int typ, char* nazev, param **parametry_v);
int zkontroluj_parametry(param *parametr, tSymbolPtr data);