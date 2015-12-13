/*
*	IFJ Projekt
*		scanner pro jazyk IFJ15
*		autor: Jakub Korgo, Martin Kruták
*		login: xkorgo01, xkruta03
*/


#include <string.h>
#include <ctype.h>
#include "tabulka_symbolu.h"
#include "instruction_list.h"
#include "define.h"
#define BUFFER_LEN 1000

typedef struct FrontaInter
{
    tSymbolPtr symbol;
    struct FrontaInter *next;
}*FrontaPtr;

typedef struct ZasobnikInstrukci
{
    tInstr *instrukce;
    struct ZasobnikInstrukci *next;	
}*ZIPtr;

int pushZI(ZIPtr* Instr_zas, tInstr* instrukce);
tInstr* popZI(ZIPtr* Instr_zas);
int pushFronta(FrontaPtr* fronta, tSymbolPtr prvek);
tSymbolPtr popFronta(FrontaPtr* fronta);
int interpret(tListOfInstr inst_list, uk_uzel global_table);
int cin_int(int *vystup, char *vstup, int *pozice);
int cin_float(double *vystup, char *vstup, int *pozice);
int cin_string(char **vystup, char *vstup, int *pozice);