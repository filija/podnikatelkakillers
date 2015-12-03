#include <stdio.h>
#include <malloc.h>
#include <string.h>
//#include "str.h"
//#include "stable.h"
//#include "ilist.h"
#include "scanner.h"
#include "define.h"
//#include "syntax.h"
#include "tabulka_symbolu.h"

typedef struct listPrecedence {
    struct listPrecedence *next;
    int s_token;
    char *promenna;
} *PrcPtr;

void generateVariable(char **var);
int pushPrc(int s_token, char* promenna, PrcPtr *list);
int popPrc(int* s_token, char** promenna, PrcPtr *list);
void emptyPrc(PrcPtr *list);
int topPrc(PrcPtr list);
int poptPrc(PrcPtr *list);
int syntax_precedencka(char **p_result);
int prec_prevod(int* token, char** attrc);
//int parse(struct symbol_table* table_hl, tList *list, ptrStack stack_hl); //
int parse(uk_uzel *GT);
int program();
int dfunkce();
int funkce();
int deklarace(tSymbolPtr *funkce_v);
int typ(int* typ_v);
int parametry(param** parametry_v);
int dparametr(param** parametry_v);
int parametr(param** parametry_v);
int slozeny();
int prikazy();
int definice(tSymbolPtr funkce_v);
int prikaz();
int prirazeni(char **ziskany);
int nacteni();
int dnacteni();
int vypis();
int dvypis();
int promenna();
int prirad(char **ziskany);
int termy(param *parametry_f);
int dterm(param *parametry_f);
int term(char **ziskany);
int load_params(param *parametry_v);