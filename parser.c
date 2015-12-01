#include <stdio.h>
#include <malloc.h>
#include <string.h>
//#include "str.h"
//#include "stable.h"
//#include "ilist.h"
#include "scanner.h"
#include "parser.h"
#include "define.h"
//#include "syntax.h"
#include "tabulka_symbolu.h"

#define getToken if ((token = getNextToken(&attr)) == LEX_ERR){return LEX_ERR;}

int token;
int countVar = 0;
uk_uzel table;
uk_uzel global_table;
// ptrStack stack;
string attr;
char *attrc;
string *params;
//tSymbolPtr hledane_id;
tSymbolPtr data; // data prubezna

//tList *List; // list instrukci


/*Funkce pro generovani instrukci*/
/*int generateInstruction(int instType, void *addr1, void *addr2, void *addr3)

{
   int result;
   tInstr I;
   I.Type = instType;
   I.addr1 = addr1;
   I.addr2 = addr2;
   I.addr3 = addr3;
   result = InsertNew(LS, I);
   return result;
}*/

void generateVariable(char **var)
{
  if ((*var = (char*) malloc(sizeof(char)*15)) == NULL) return;
  memset(*var,0,15);
  sprintf (*var, "$%d", countVar);
  countVar ++;
}
//stringy uvnitr kopirovat pro ID, u ostatnich neni relevatni a muze byt string NULL
int pushPrc(int s_token, char* promenna, PrcPtr *list) {	//navratova hodnota funkce je jestli byla vnitrni chyba, nebo ne
	PrcPtr tmpItem;
	if ( (tmpItem = malloc(sizeof(struct listPrecedence))) == NULL )return INTERNAL_ERR;
	tmpItem->s_token = s_token;
	tmpItem->promenna = promenna;
	tmpItem->next = *list;
	*list = tmpItem;
	return 0;
}
int popPrc (int* s_token, char* promenna, PrcPtr *list){	//navratova hodnota, jestli neni vstupni list NULL
	if (*list == NULL) return -1;
	promenna = (*list)->promenna;
	*s_token = (*list)->s_token;
	PrcPtr tmp = *list;
	*list = (*list)->next;
	free(tmp);
	return 0;
}
void emptyPrc(PrcPtr *list){	//nejspise se nesmaze prvni, proverit
	if (*list == NULL){
		return;
	}
	emptyPrc(&((*list)->next));
	free((*list)->promenna);
	free(*list);
}
int topPrc(PrcPtr list){	//vraci hodnotu adresace do tabulky z terminal_listu
	if (list == NULL) return -1;
	return list->s_token;
}

int poptPrc(PrcPtr *list){
	if (*list == NULL) return -1;
	PrcPtr tmp = *list;
	*list = (*list)->next;
	free(tmp);
	return 0;
}

int Table[14][14] =
// sloupec - vstupni token, radek - znak na zasobniku, tabulka jeste neni opravena? Nezda se mi uvolnovani pameti!
{ 
//		   ID 	  (		  )       +        -      *       /       ==      <=       <      !=      >=      >       $
/*ID*/	{SHERR,	SHERR,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*(*/	{SHIFT,	SHIFT,	HADLE,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHERR},
/*)*/	{SHERR,	SHERR,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*+*/	{SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*-*/	{SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/***/	{SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*/*/	{SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*==*/	{SHIFT,	SHIFT,	RPLCE,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*<=*/	{SHIFT,	SHIFT,	RPLCE,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*<*/	{SHIFT,	SHIFT,	RPLCE,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*!=*/	{SHIFT,	SHIFT,	RPLCE,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*>=*/	{SHIFT,	SHIFT,	RPLCE,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*>*/	{SHIFT,	SHIFT,	RPLCE,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*$*/	{SHIFT,	SHIFT,	SHERR,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHERR}

};

int syntax_precedencka(){	//neni potreba uvolnovat znaminka atd... neukladaji stringy
int tok1;
int tok2;
int tok3;
char* atr1 = NULL;
char* atr2 = NULL;
char* atr3 = NULL;

PrcPtr PSAlist = NULL;	//Zasobnik vsech ulozenych prvku (pushdown zasobnik)
PrcPtr terminal_list = NULL;	//zasobik prvku, ktere jsou pro nas relevantni pro porovnavani posledniho znaku na zasobniku, (v prezentacich se za ne uklada "<")
int vysl_prc = 0;		//zde se uklada navratova hodnota z tabulky
int terminalTop = S_DOLAR;	//hodnota/nazev terminalu na vrcholu zasobniku pro PSA
int nacteny = S_DOLAR;		//hodnota/nazev nacitaneho tokenu
//samotny kod PSA (precedencni SA)
//pak dodelat vsude error handling
//token prijde od LL, vzdy se dalsi token nacita po jeho zpracovani, ne pred nim!
//push na teminallist DOLAR
pushPrc(S_DOLAR, NULL, &terminal_list);
string_from_char(&attrc, &attr);
nacteny = prec_prevod(&token, &attrc);
if (Table[terminalTop][nacteny] == SHERR) return SYN_ERR;	//Vyraz musi neco obsahovat
do{
	vysl_prc = Table[terminalTop][nacteny];
	switch (vysl_prc){
		case SHIFT:
			pushPrc(token, attrc, &PSAlist);
			pushPrc(nacteny, attrc, &terminal_list);
			getToken;
			string_from_char(&attrc, &attr);
			nacteny = prec_prevod(&token, &attrc);
			break;
		case HADLE:
			poptPrc(&terminal_list);
			if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
			if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
			if(tok1 != ID) return SYN_ERR; //nejaka chyba
			if(tok2 != L_ZAVORKA) return SYN_ERR;
			pushPrc(tok1, atr1, &PSAlist);
			//vytahnout levou zavorku z terminal_list (pop(terminallist))
			//2x pop a nekam si ulozit hodnoty z PSAlist
			//zkontrolovat jestli odpovida tvar: "(ID)", pokud ne vratit spravny error
			//pokud tvar odpovida, pushnout ID na PSAlist
			getToken;
			string_from_char(&attrc, &attr);
			pushPrc(S_ID, NULL, &terminal_list);
			nacteny = prec_prevod(&token, &attrc);			
			break;
		case RPLCE:
			switch(terminalTop){
				case S_ID:
					poptPrc(&terminal_list);
					//pop terminal list, nic vic. Pro nas je to jak kdyby prevod ID -> E
					break;
				case S_PLUS:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != PLUS) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					// pop(terminallist) -> zpracovali jsme terminal na topu
					// 3x pop PSAlist a kazdy pop si ulozit, nezapomenout ze se to popuje od zadu!!! a-b vs b-a
					// zkotrolovat jestli odpovida tvar ID + ID, pokud ne vyhodit error (napr mohlo by prijit + ID +, v pripade ze by se redukce provedla pred zadanim promenne)
					// pokud vse odpovida:
					// vygenerovat promennou do ktere se ulozi vysledek
					generateVariable(&atr2); //v atr2 je zarucene NULL, pokud se to sem dostalo, proto tuto promennou muzeme pouzit
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_MINUS:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != MINUS) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					generateVariable(&atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_NASOBENI:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != NASOBENI) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					generateVariable(&atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_DELENI:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != DELENI) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					generateVariable(&atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_ROVNA_SE:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != ROVNA_SE) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					generateVariable(&atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_MENE_NEBO_ROVNO:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != MENE_NEBO_ROVNO) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					generateVariable(&atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_MENE:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != MENE) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					generateVariable(&atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_NEROVNOST:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != NEROVNOST) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					generateVariable(&atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_VICE_NEBO_ROVNO:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != VICE_NEBO_ROVNO) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					generateVariable(&atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_VICE:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != VICE) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					generateVariable(&atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, &PSAlist);
					break;
				default:
					printf("Toto by se nemelo stat. nhlasit!\n");
					//stdout ze tady by to nikdy dojit nemelo a najit v jakem pripade se to stalo :)
					break;
			}
			break;
		case SHERR:
			if (terminalTop == S_L_ZAVORKA) return SYN_ERR;
			nacteny = S_DOLAR;
	}

	terminalTop = topPrc(terminal_list);
}while (nacteny != S_DOLAR || terminalTop != S_DOLAR);
return IS_OK;
//navrat hodnoty na pop(PSAlist), tam bude ta promenna ve ktere je ulozeny vysledek vyrazu
//konec PSA
}


int prec_prevod(int* token, char** attrc){
	switch(*token){
		case INT_V:
			//attr se prevede na int hodnotu
			//hodnota se ulozi s unikatnim identifikatorem do lokalni tabulky (+ dalsi dulezite informace, jako ze je to int, ze je to promenna atd...)
			//unikatni identifikator se zapise do attr
			generateVariable(attrc);		
			//zmeni se hodnota tokenu na ID
			*token = ID;
			return S_ID;
		case DOUBLE_V:
			//attr se prevede na double hodnotu
			//hodnota se ulozi s unikatnim identifikatorem do lokalni tabulky
			//unikatni identifikator se zapise do attr
			generateVariable(attrc);
			//zmeni se hodnota tokenu na ID
			*token = ID;
			return S_ID;
		case STRING_V:
			//attr se zkopiruje na novy oddil pameti (kdyby se zkopirval ukazatel, tak po zmene attr, by jsme ztratili "hodnotu")
			//hodnota se ulozi s unikatnim identifikatorem do lokalni tabulky
			//unikatni identifikator se zapise do attr
			generateVariable(attrc);
			//zmeni se hodnota tokenu na ID
			*token = ID;
			return S_ID;
		case ID:
			return S_ID;
		case L_ZAVORKA:
			return S_L_ZAVORKA;
		case P_ZAVORKA:
			return S_P_ZAVORKA;
		case PLUS:
			return S_PLUS;
		case MINUS:
			return S_MINUS;
		case NASOBENI:
			return S_NASOBENI;
		case DELENI:
			return S_DELENI;
		case ROVNA_SE:
			return S_ROVNA_SE;
		case MENE_NEBO_ROVNO:
			return S_MENE_NEBO_ROVNO;
		case MENE:
			return S_MENE;
		case NEROVNOST:
			return S_NEROVNOST;
		case VICE_NEBO_ROVNO:
			return S_VICE_NEBO_ROVNO;
		case VICE:
			return S_VICE;
		default:	//dolar
			return S_DOLAR;
	}
}

/*Hlavni funkce parseru*/
//int parse(struct symbol_table* table_hl, tList *list, ptrStack stack_hl){
int parse(uk_uzel *GT){ 
  int result;
  global_table = *GT;/*Nastaveni tabulky*/
//  Stack = stack_hl;/*Nastaveni zasobniku*/
  strInit(&attr);
  
//  LS = list;/*Nastaveni listu*/
  
  /*Kontrola prvniho tokenu*/
  if ((token = getNextToken(&attr)) == LEX_ERR)
     result = LEX_ERR;
  else
     result = program();/*Zavolani kontoly syntaxe programu*/
  /*Uvolneni retezcu*/
  strFree(&attr);
  /*if (str_parameters != NULL){	//co to ma byt za kod?
    strFree(str_parameters);
    free(str_parameters);
    }	
  */
  return result;
}

// zacatek 
int program(){
	/*<PROGRAM>*/
	int outcome;
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
		case FILEEND:
			/*PROGRAM → DFUNKCE eof*/
			outcome = dfunkce();
			if (outcome != IS_OK) return outcome;	
			if (token != FILEEND) return SYN_ERR;		
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int dfunkce(){
		/*<DFUNKCE>*/
	int outcome;
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/*DFUNKCE → FUNKCE DFUNKCE*/
			outcome = funkce();
			if (outcome != IS_OK) return outcome;
			outcome = dfunkce();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case FILEEND:
			/*DFUNKCE → ε*/
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int funkce(){
	//nekde pri deklaraci funkce, se bude kontrolovat, jeslti nazev funkce neni shodny s klicovym slovem
		/*<FUNKCE>*/
	int outcome;
	tSymbolPtr funkce_v = NULL;
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/*FUNKCE → DEKLARACE DEFINICE*/
			outcome = deklarace(&funkce_v);
			if (outcome != IS_OK) return outcome;
			outcome = definice(funkce_v);
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}	
}

int deklarace(tSymbolPtr *funkce_v){
			/*<DEKLARACE>*/
	int outcome;
	int typ_v=0;
	char* nazev=NULL;
	param* parametry_v=NULL;
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/*DEKLARACE → TYP id lz PARAMETRY pz*/
			outcome = typ(&typ_v);
			if (outcome != IS_OK) return outcome;
			if (token != ID) return SYN_ERR;
			string_from_char(&nazev, &attr);
			getToken
			if (token != L_ZAVORKA) return SYN_ERR;
			getToken
			outcome = parametry(&parametry_v);
			if (outcome != IS_OK) return outcome;
			if (token != P_ZAVORKA) return SYN_ERR;
			tSymbolPtr hledany;
			hledany = najdi_v_tabulce (global_table, nazev);
			if (hledany != NULL){
				if (hledany->typ != typ_v) return SEM_ERR;
				outcome = zkontroluj_parametry(parametry_v, hledany);
				if (outcome != IS_OK) return outcome;
			}
			else {
				inicializuj_data(&hledany); //alokuje pamet a inicializuje hodnoty
				hledany->typ = typ_v;
				hledany->verze = 1;
				hledany->parametry = parametry_v;
				vloz_do_tabulky (&global_table, nazev, hledany);
			}
			*funkce_v = hledany;
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}	
}

int typ(int* typ_v){
	/* TYP */
	switch(token){
		case INT:
			*typ_v = 1;
			/*TYP → int*/
			getToken
			return IS_OK;
			break;
		case DOUBLE:
			*typ_v = 2;
			/*TYP → double*/
			getToken
			return IS_OK;
			break;
		case STRING:
			*typ_v = 3;
			/*TYP → string*/
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int parametry(param** parametry_v){
		/*<PARAMETRY>*/
	int outcome;
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
		/* PARAMETRY → PARAMETR DPARAMETR */
			outcome = parametr(parametry_v);
			if (outcome != IS_OK) return outcome;
			outcome = dparametr(parametry_v);
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case P_ZAVORKA:
		/* PARAMETRY → ε */
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int dparametr(param** parametry_v){
	/*<DPARAMETRY>*/
	int outcome;
	switch(token){
		case P_ZAVORKA:
			/*DPARAMETR → ε*/
			return IS_OK;
			break;
		case CARKA:
			/*DPARAMETR → carka PARAMETR DPARAMETR*/
			getToken
			outcome = parametr(parametry_v);
			if (outcome != IS_OK) return outcome;
			outcome = dparametr(parametry_v);
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int parametr(param** parametry_v){
	/* PARAMETR */
	int outcome;
	int typ_v;
	char* nazev=NULL;

	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/*PARAMETR → TYP id*/
			outcome = typ(&typ_v);
			if (outcome != IS_OK) return outcome;
			if (token != ID) return SYN_ERR;
			string_from_char(&nazev, &attr);
			vloz_do_parametru(typ_v,nazev,parametry_v);
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int slozeny(){
	/* SLOZENY → lsz PRIKAZY psz */
	int outcome;
	switch(token){
		case LS_ZAVORKA:
			getToken
			outcome = prikazy();
			if (outcome != IS_OK) return outcome;
			if (token != PS_ZAVORKA) return SYN_ERR;
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int prikazy(){
	/* PRIKAZY → PRIKAZ PRIKAZY */
	int outcome;

	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
		case ID:
		case IF:
		case FOR:
		case RETURN:
		case CIN:
		case COUT:
		case AUTO:
			outcome = prikaz();
			if (outcome != IS_OK) return outcome;
			outcome = prikazy();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case PS_ZAVORKA:
		/* PRIKAZY → ε */
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}	

int definice(tSymbolPtr funkce_v){
	/* DEFINICE → SLOZENY */
	int outcome;
	switch(token){
		case LS_ZAVORKA:
			if (funkce_v->defined == 1) return SEM_ERR;
			funkce_v->defined = 1;
			//vytvorit instrukci label a dat ji definici funkci
			//vytvorit lokalni tabulku a dat ji definici funkce
			//vyskladat parametry do lokalni tabulky
			outcome = slozeny();
			if (outcome != IS_OK) return outcome;
			//uzavrit lokalni tabulku (ukazatel v parser.c = NULL)
			return IS_OK;
			break;
		case STREDNIK:
		/* DEFINICE → strednik */
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int prikaz(){
	/* PRIKAZ → PROMENNA strednik */
	int outcome;
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
		case AUTO:
			outcome = promenna();
			if (outcome != IS_OK) return outcome;
			if (token != STREDNIK) return SYN_ERR;
			getToken
			return IS_OK;
			break;
		/* PRIKAZ → id rovnitko PRIRAZENI strednik */
		case ID:
			getToken
			if (token != PRIRAZENI) return SYN_ERR;			
			getToken
			outcome = prirazeni();
			if (outcome != IS_OK) return outcome;
			if (token != STREDNIK) return SYN_ERR;
			getToken
			return IS_OK;
			break;
		/* PRIKAZ → if lz expr pz SLOZENY else SLOZENY */
		case IF:
			getToken
			if (token != L_ZAVORKA) return SYN_ERR;
			getToken
			// expr
			outcome = syntax_precedencka();
			if (outcome != IS_OK) return outcome;
			// expr dava nacteny dalsi token
			if (token != P_ZAVORKA) return SYN_ERR;
			getToken
			outcome = slozeny();
			if (outcome != IS_OK) return outcome;
			if (token != ELSE) return SYN_ERR;
			getToken
			outcome = slozeny();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		/* PRIKAZ → for lz PROMENNA strednik expr strednik id rovnitko expr pz SLOZENY */
		case FOR:
			getToken
			if (token != L_ZAVORKA) return SYN_ERR;
			getToken
			outcome = promenna();
			if (outcome != IS_OK) return outcome;
			if (token != STREDNIK) return SYN_ERR;
			getToken
			// expr
			outcome = syntax_precedencka();
			if (outcome != IS_OK) return outcome;
			// expr dava nacteny dalsi token
			if (token != STREDNIK) return SYN_ERR;
			getToken
			if (token != ID) return SYN_ERR;
			getToken
			if (token != PRIRAZENI) return SYN_ERR;
			getToken
			// expr
			outcome = syntax_precedencka();
			if (outcome != IS_OK) return outcome;
			// expr dava nacteny dalsi token
			if (token != P_ZAVORKA) return SYN_ERR;
			getToken
			outcome = slozeny();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		/* PRIKAZ → return expr strednik */
		case RETURN:
			getToken
			// expr
			outcome = syntax_precedencka();
			if (outcome != IS_OK) return outcome;
			// expr dava nacteny dalsi token
			if (token != STREDNIK) return SYN_ERR;
			getToken
			return IS_OK;
			break;
		/* PRIKAZ → cin NACTENI strednik */
		case CIN:
			getToken
			outcome = nacteni();
			if (outcome != IS_OK) return outcome;
			if (token != STREDNIK) return SYN_ERR;
			getToken
			return IS_OK;
			break;
		/* PRIKAZ → cout VYPIS strednik */
		case COUT:
			getToken
			outcome = vypis();
			if (outcome != IS_OK) return outcome;
			if (token != STREDNIK) return SYN_ERR;
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int prirazeni(){	//case ID and id je jmeno funkce... na tomto nam bude padat kontrola
	int outcome;
	switch(token){
		/* PRIRAZENI → id lz TERMY pz */
		case ID:
			getToken
			if (token != L_ZAVORKA) return SYN_ERR;
			getToken
			outcome = termy();
			if (outcome != IS_OK) return outcome;
			if (token != P_ZAVORKA) return SYN_ERR;
			getToken
			return IS_OK;
			break;
		// case expresiion <<< PRIRAZENI → expr
		default:
			outcome = syntax_precedencka();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
	}
}

int nacteni(){
	int outcome;
	switch(token){
		/* NACTENI → nacti id DNACTENI */
		case CTENI:
			getToken
			if (token != ID) return SYN_ERR;
			getToken 
			outcome = dnacteni();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int dnacteni(){
	int outcome;
	switch(token){
		/* DNACTENI → NACTENI */
		case CTENI:
			outcome = nacteni();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		/* DNACTENI → ε */
		case STREDNIK:
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int vypis(){
	int outcome;
	switch(token){
		/* VYPIS → pis TERM DVYPIS */
		case ZAPIS:
			getToken
			outcome = term();
			if (outcome != IS_OK) return outcome;
			outcome = dvypis();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int dvypis(){
	int outcome;
	switch(token){
		/* DVYPIS → VYPIS */
		case ZAPIS:
			outcome = vypis();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		/* DVYPIS → ε */
		case STREDNIK:
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int promenna(){
	int outcome;
	int typ_v;
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/* PROMENNA → TYP id PRIRAD */
			outcome = typ(&typ_v);
			if (outcome != IS_OK) return outcome;
			if (token != ID) return SYN_ERR;
			getToken
			outcome = prirad();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case AUTO:
		 /* PROMENNA → auto id rovnitko expr */
			getToken
			if (token != ID) return SYN_ERR;
			getToken
			if (token != PRIRAZENI) return SYN_ERR;
			getToken
			//expression 
			outcome = syntax_precedencka();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
		 	return SYN_ERR;
		 	break;
	}
}

int prirad(){
	int outcome;
	switch(token){
		case PRIRAZENI:
		/* PRIRAD → rovnitko expr */	
			getToken
			//expr
			outcome = syntax_precedencka();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case STREDNIK:
		/* PRIRAD → ε */
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int termy(){
	int outcome;
	switch(token){
		/* TERMY → TERM DTERM */
		case ID:
		case INT_V:
		case DOUBLE_V:
		case STRING_V:
			outcome = term();
			if (outcome != IS_OK) return outcome;
			outcome = dterm();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case P_ZAVORKA:
		/* TERMY → ε */
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int dterm(){
	int outcome;
	switch(token){
		case P_ZAVORKA:
		/* DTERM → ε */
			return IS_OK;
			break;
		/* DTERM → carka TERM DTERM */
		case CARKA:
			getToken
			outcome = term();
			if (outcome != IS_OK) return outcome;
			outcome = dterm();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int term(){
	switch(token){
		/* TERM → id */
		case ID:
			getToken
			return IS_OK;
			break;
		case INT_V:
			/* TERM → literal */
			getToken
			return IS_OK;
			break;
		case DOUBLE_V:
			/* TERM → literal */
			getToken
			return IS_OK;
			break;
		case STRING_V:
			/* TERM → literal */
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}