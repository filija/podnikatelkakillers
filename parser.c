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
//#include "tabulka_symbolu.h"

#define getToken if ((token = getNextToken(&attr)) == LEX_ERR){return LEX_ERR;}

int token;
int countVar = 0;
//struct symbol_table *table;
//struct symbol_table *glob_table;
// ptrStack stack;
string attr;
char *attrc;
string *params;
//tSymbolPtr hledane_id;
//tSymbolPtr data; // data prubezna

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

void generateVariable(char *var)
{
  if ((var = (char*) malloc(sizeof(char)*15)) == NULL) return;
  memset(var,0,15);
  sprintf (var, "$%d", countVar);
  countVar ++;
}
//stringy uvnitr kopirovat pro ID, u ostatnich neni relevatni a muze byt string NULL
int pushPrc(int s_token, char* promenna, PrcPtr list) {	//navratova hodnota funkce je jestli byla vnitrni chyba, nebo ne
	PrcPtr tmpItem;
	if ( (tmpItem = malloc(sizeof(struct listPrecedence))) == NULL )return INTERNAL_ERR;
	tmpItem->s_token = s_token;
	tmpItem->promenna = promenna;
	tmpItem->next = list;
	list = tmpItem;
	return 0;
}
int popPrc(int* s_token, char* promenna, PrcPtr list){	//navratova hodnota, jestli neni vstupni list NULL
	if (list == NULL) return -1;
	promenna = list->promenna;
	*s_token = list->s_token;
	PrcPtr tmp = list;
	list = list->next;
	free(tmp);
	return 0;
}
void emptyPrc(PrcPtr list){	//nejspise se nesmaze prvni, proverit
	if (list == NULL){
		return;
	}
	emptyPrc(list->next);
	free(list->promenna);
	free(list);
}
int topPrc(PrcPtr list){	//vraci hodnotu adresace do tabulky z terminal_listu
	if (list == NULL) return -1;
	return list->s_token;
}

int poptPrc(PrcPtr list){
	if (list == NULL) return -1;
	PrcPtr tmp = list;
	list = list->next;
	free(tmp);
	return 0;
}

int Table[14][14] =
// sloupec - vstupni token, radek - znak na zasobniku, tabulka jeste neni opravena!!
{ 
//		   ID 	  (		  )       +        -      *       /       ==      <=       <      !=      >=      >       $
/*ID*/	{RPLCE,	SHERR,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
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

int syntax_precedencka(){
int tok1;
int tok2;
int tok3;
char* atr1 = NULL;
char* atr2 = NULL;
char* atr3 = NULL;

PrcPtr PSAlist = NULL;	//Zasobnik vsech ulozenych prvku (pushdown zasobnik)
PrcPtr terminal_list = NULL;	//zasobik prvku, ktere jsou pro nas relevantni pro porovnavani posledniho znaku na zasobniku, (v prezentacich se za ne uklada "<")
int vysl_prc = 0;		//zde se uklada navratova hodnota z tabulky
int terminalTop = 13;	//hodnota/nazev terminalu na vrcholu zasobniku pro PSA
int nacteny = 13;		//hodnota/nazev nacitaneho tokenu
//samotny kod PSA (precedencni SA)
//pak dodelat vsude error handling
//token prijde od LL, vzdy se dalsi token nacita po jeho zpracovani, ne pred nim!
//push na teminallist DOLAR
pushPrc(13, NULL, terminal_list);
string_from_char(attrc, &attr);
nacteny = prec_prevod(&token, attrc);	//zase otazka jestli se ma pouzit dereference, nebo attr sam o sbe je ukazatel, pak se podivat a poresit na vsech mistech
do{
	vysl_prc = Table[terminalTop][nacteny];
	switch (vysl_prc){
		case SHIFT:
			pushPrc(token, attrc, PSAlist);
			pushPrc(nacteny, attrc, terminal_list);
			getToken;
			string_from_char(attrc, &attr);
			nacteny = prec_prevod(&token, attrc);
			break;
		case HADLE:
			poptPrc(terminal_list);
			popPrc(&tok1, atr1, PSAlist);
			popPrc(&tok2, atr2, PSAlist);
			if(tok1 != ID) return SYN_ERR; //nejaka chyba
			if(tok2 != L_ZAVORKA) return SYN_ERR;
			pushPrc(tok1, atr1, PSAlist);
			free(atr2);
			//vytahnout levou zavorku z terminal_list (pop(terminallist))
			//2x pop a nekam si ulozit hodnoty z PSAlist
			//zkontrolovat jestli odpovida tvar: "(ID)", pokud ne vratit spravny error
			//pokud tvar odpovida, pushnout ID na PSAlist
			getToken;
			string_from_char(attrc, &attr);
			nacteny = prec_prevod(&token, attrc);			
			break;
		case RPLCE:
			switch(terminalTop){
				case ID:
					poptPrc(terminal_list);
					//pop terminal list, nic vic. Pro nas je to jak kdyby prevod ID -> E
					break;
				case PLUS:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != PLUS) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba

					// pop(terminallist) -> zpracovali jsme terminal na topu
					// 3x pop PSAlist a kazdy pop si ulozit, nezapomenout ze se to popuje od zadu!!! a-b vs b-a
					// zkotrolovat jestli odpovida tvar ID + ID, pokud ne vyhodit error (napr mohlo by prijit + ID +, v pripade ze by se redukce provedla pred zadanim promenne)
					// pokud vse odpovida:
					// vygenerovat promennou do ktere se ulozi vysledek
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				case MINUS:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != MINUS) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				case NASOBENI:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != NASOBENI) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				case DELENI:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != DELENI) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				case ROVNA_SE:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != ROVNA_SE) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				case MENE_NEBO_ROVNO:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != MENE_NEBO_ROVNO) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				case MENE:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != MENE) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				case NEROVNOST:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != NEROVNOST) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				case VICE_NEBO_ROVNO:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != VICE_NEBO_ROVNO) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				case VICE:
					poptPrc(terminal_list);
					popPrc(&tok1, atr1, PSAlist);
					popPrc(&tok2, atr2, PSAlist);
					popPrc(&tok3, atr3, PSAlist);
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != VICE) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					free(atr2);	//recyklujeme promenne, tuto informaci nepotrebujeme
					generateVariable(atr2);
					//nacpat promennou do lokalni tabulky
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					pushPrc(ID, atr2, PSAlist);
					break;
				default:
					//stdout ze tady by to nikdy dojit nemelo a najit v jakem pripade se to stalo :)
					break;
			}
			break;
		case SHERR:
			nacteny = 13;
	}

	terminalTop = topPrc(terminal_list);
}while (nacteny != DOLAR || terminalTop != DOLAR);
return 0;
//navrat hodnoty na pop(PSAlist), tam bude ta promenna ve ktere je ulozeny vysledek vyrazu
//konec PSA
}


int prec_prevod(int* token, char* attrc){ //ma to byt ukazatel, nebo string sam o sobe je ukazatel na strukturu?
	switch(*token){
		case INT_V:
			//attr se prevede na int hodnotu
			//hodnota se ulozi s unikatnim identifikatorem do lokalni tabulky (+ dalsi dulezite informace, jako ze je to int, ze je to promenna atd...)
			//unikatni identifikator se zapise do attr
			generateVariable(attrc);		
			//zmeni se hodnota tokenu na ID
			*token = ID;
			return 0;
		case DOUBLE_V:
			//attr se prevede na double hodnotu
			//hodnota se ulozi s unikatnim identifikatorem do lokalni tabulky
			//unikatni identifikator se zapise do attr
			generateVariable(attrc);
			//zmeni se hodnota tokenu na ID
			*token = ID;
			return 0;
		case STRING_V:
			//attr se zkopiruje na novy oddil pameti (kdyby se zkopirval ukazatel, tak po zmene attr, by jsme ztratili "hodnotu")
			//hodnota se ulozi s unikatnim identifikatorem do lokalni tabulky
			//unikatni identifikator se zapise do attr
			generateVariable(attrc);
			//zmeni se hodnota tokenu na ID
			*token = ID;
			return 0;
		case ID:
			return 0;
		case L_ZAVORKA:
			return 1;
		case P_ZAVORKA:
			return 2;
		case PLUS:
			return 3;
		case MINUS:
			return 4;
		case NASOBENI:
			return 5;
		case DELENI:
			return 6;
		case ROVNA_SE:
			return 7;
		case MENE_NEBO_ROVNO:
			return 8;
		case MENE:
			return 9;
		case NEROVNOST:
			return 10;
		case VICE_NEBO_ROVNO:
			return 11;
		case VICE:
			return 12;
		default:	//dolar
			return 13;
	}
}

/*Hlavni funkce parseru*/
//int parse(struct symbol_table* table_hl, tList *list, ptrStack stack_hl){
int parse(){ 
  int result;
//  table = table_hl;/*Nastaveni tabulky*/
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
	
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/*FUNKCE → DEKLARACE DEFINICE*/
			outcome = deklarace();
			if (outcome != IS_OK) return outcome;
			outcome = definice();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}	
}

int deklarace(){
			/*<DEKLARACE>*/
	int outcome;
	
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/*DEKLARACE → PARAMETR lz PARAMETRY pz*/
			outcome = parametr();
			if (outcome != IS_OK) return outcome;
			if (token != L_ZAVORKA) return SYN_ERR;
			getToken
			outcome = parametry();
			if (outcome != IS_OK) return outcome;
			if (token != P_ZAVORKA) return SYN_ERR;
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}	
}

int typ(){
	/* TYP */
	switch(token){
		case INT:
			/*TYP → int*/
			getToken
			return IS_OK;
			break;
		case DOUBLE:
			/*TYP → double*/
			getToken
			return IS_OK;
			break;
		case STRING:
			/*TYP → string*/
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int parametry(){
		/*<PARAMETRY>*/
	int outcome;
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
		/* PARAMETRY → PARAMETR DPARAMETR */
			outcome = parametr();
			if (outcome != IS_OK) return outcome;
			outcome = dparametr();
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

int dparametr(){
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
			outcome = parametr();
			if (outcome != IS_OK) return outcome;
			outcome = dparametr();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int parametr(){
	/* PARAMETR */
	int outcome;

	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/*PARAMETR → TYP id*/
			outcome = typ();
			if (outcome != IS_OK) return outcome;
			if (token != ID) return SYN_ERR;
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

int definice(){
	/* DEFINICE → SLOZENY */
	int outcome;
	switch(token){
		case LS_ZAVORKA:
			outcome = slozeny();
			if (outcome != IS_OK) return outcome;
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
			// expr dava nacteny dalsi token
			if (token != STREDNIK) return SYN_ERR;
			getToken
			if (token != ID) return SYN_ERR;
			getToken
			if (token != PRIRAZENI) return SYN_ERR;
			getToken
			// expr
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

int prirazeni(){
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
			return SYN_ERR;
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
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/* PROMENNA → PARAMETR PRIRAD */
			outcome = parametr();
			if (outcome != IS_OK) return outcome;
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
			return IS_OK;
			break;
		default:
		 	return SYN_ERR;
		 	break;
	}
}

int prirad(){
	switch(token){
		case PRIRAZENI:
		/* PRIRAD → rovnitko expr */	
			getToken
			//expr
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