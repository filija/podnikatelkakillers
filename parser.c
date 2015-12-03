#include "parser.h"

#define getToken if ((token = getNextToken(&attr)) == LEX_ERR){return LEX_ERR;}

int token;
int countVar = 0;
tStackPtr table = NULL;
uk_uzel global_table = NULL;
// ptrStack stack;
string attr;
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


void print_koren_udu(uk_uzel tabulka){
	if(tabulka == NULL) return;
	printf("Jmeno a hodnota(int) je >>> %s %i %f\n", tabulka->symbol, tabulka->data->value.i, tabulka->data->value.d);
	print_koren_udu(tabulka->LPtr);
	print_koren_udu(tabulka->RPtr);
}

void generateVariable(char **var)
{
  if ((*var = (char*) malloc(sizeof(char)*15)) == NULL) return;
  memset(*var,0,15);
  sprintf (*var, "$%i", countVar);
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
int popPrc (int* s_token, char** promenna, PrcPtr *list){	//navratova hodnota, jestli neni vstupni list NULL
	if (*list == NULL) return -1;
	*promenna = (*list)->promenna;
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

int syntax_precedencka(char **p_result){	//neni potreba uvolnovat znaminka atd... neukladaji stringy
int tok1;
int tok2;
int tok3;
int type_var;
char* atr1 = NULL;
char* atr2 = NULL;
char* atr3 = NULL;
char *attrc = NULL;
tSymbolPtr vkladany1 = NULL;
tSymbolPtr vkladany2 = NULL;
tSymbolPtr vkladany3 = NULL;
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
			if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
			if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
			if(tok1 != ID) return SYN_ERR; //nejaka chyba
			if(tok2 != L_ZAVORKA) return SYN_ERR;
			pushPrc(tok1, atr1, &PSAlist);
			//vytahnout levou zavorku z terminal_list (pop(terminallist))
			//2x pop a nekam si ulozit hodnoty z PSAlist
			//zkontrolovat jestli odpovida tvar: "(ID)", pokud ne vratit spravny error
			//pokud tvar odpovida, pushnout ID na PSAlist
			getToken;
			pushPrc(S_ID, NULL, &terminal_list);
			string_from_char(&attrc, &attr);
			nacteny = prec_prevod(&token, &attrc);			
			break;
		case RPLCE:
			vkladany1 = NULL;
			vkladany2 = NULL;
			vkladany3 = NULL;
			switch(terminalTop){
				case S_ID:
					poptPrc(&terminal_list);
					//pop terminal list, nic vic. Pro nas je to jak kdyby prevod ID -> E
					break;
				case S_PLUS:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR;
					if(tok2 != PLUS) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR;
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if (vkladany1->typ == INT_V && vkladany3->typ == INT_V) type_var = 1;
					else if (vkladany1->typ == DOUBLE_V && vkladany3->typ == DOUBLE_V) type_var = 2;
					else if (vkladany1->typ == DOUBLE_V && vkladany3->typ == INT_V) type_var = 2;
					else if (vkladany1->typ == INT_V && vkladany3->typ == DOUBLE_V) type_var = 2;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 - vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s + %s\n", atr2, atr3, atr1);
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_MINUS:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != MINUS) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if (vkladany1->typ == INT_V && vkladany3->typ == INT_V) type_var = 1;
					else if (vkladany1->typ == DOUBLE_V && vkladany3->typ == DOUBLE_V) type_var = 2;
					else if (vkladany1->typ == DOUBLE_V && vkladany3->typ == INT_V) type_var = 2;
					else if (vkladany1->typ == INT_V && vkladany3->typ == DOUBLE_V) type_var = 2;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 - vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s - %s\n", atr2, atr3, atr1);
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_NASOBENI:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != NASOBENI) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if (vkladany1->typ == INT_V && vkladany3->typ == INT_V) type_var = 1;
					else if (vkladany1->typ == DOUBLE_V && vkladany3->typ == DOUBLE_V) type_var = 2;
					else if (vkladany1->typ == DOUBLE_V && vkladany3->typ == INT_V) type_var = 2;
					else if (vkladany1->typ == INT_V && vkladany3->typ == DOUBLE_V) type_var = 2;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 * vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s * %s\n", atr2, atr3, atr1);
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_DELENI:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != DELENI) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if (vkladany1->typ == INT_V && vkladany3->typ == INT_V) type_var = 1;
					else if (vkladany1->typ == DOUBLE_V && vkladany3->typ == DOUBLE_V) type_var = 2;
					else if (vkladany1->typ == DOUBLE_V && vkladany3->typ == INT_V) type_var = 2;
					else if (vkladany1->typ == INT_V && vkladany3->typ == DOUBLE_V) type_var = 2;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 / vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s / %s\n", atr2, atr3, atr1);
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_ROVNA_SE:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != ROVNA_SE) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if ((vkladany1->typ == INT_V || vkladany1->typ == DOUBLE_V) && (vkladany3->typ == INT_V || vkladany3->typ == DOUBLE_V)) type_var = 1;
					else if (vkladany1->typ == STRING_V && vkladany3->typ == STRING_V) type_var = 1;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 == vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s == %s\n", atr2, atr3, atr1);
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_MENE_NEBO_ROVNO:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != MENE_NEBO_ROVNO) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if ((vkladany1->typ == INT_V || vkladany1->typ == DOUBLE_V) && (vkladany3->typ == INT_V || vkladany3->typ == DOUBLE_V)) type_var = 1;
					else if (vkladany1->typ == STRING_V && vkladany3->typ == STRING_V) type_var = 1;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 == vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s <= %s\n", atr2, atr3, atr1);
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_MENE:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != MENE) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if ((vkladany1->typ == INT_V || vkladany1->typ == DOUBLE_V) && (vkladany3->typ == INT_V || vkladany3->typ == DOUBLE_V)) type_var = 1;
					else if (vkladany1->typ == STRING_V && vkladany3->typ == STRING_V) type_var = 1;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 == vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s < %s\n", atr2, atr3, atr1);
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_NEROVNOST:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != NEROVNOST) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if ((vkladany1->typ == INT_V || vkladany1->typ == DOUBLE_V) && (vkladany3->typ == INT_V || vkladany3->typ == DOUBLE_V)) type_var = 1;
					else if (vkladany1->typ == STRING_V && vkladany3->typ == STRING_V) type_var = 1;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 == vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s != %s\n", atr2, atr3, atr1);
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_VICE_NEBO_ROVNO:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != VICE_NEBO_ROVNO) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if ((vkladany1->typ == INT_V || vkladany1->typ == DOUBLE_V) && (vkladany3->typ == INT_V || vkladany3->typ == DOUBLE_V)) type_var = 1;
					else if (vkladany1->typ == STRING_V && vkladany3->typ == STRING_V) type_var = 1;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 == vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s >= %s\n", atr2, atr3, atr1);
					pushPrc(ID, atr2, &PSAlist);
					break;
				case S_VICE:
					poptPrc(&terminal_list);
					if (popPrc(&tok1, &atr1, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok2, &atr2, &PSAlist)) return SYN_ERR;
					if (popPrc(&tok3, &atr3, &PSAlist)) return SYN_ERR;
					if(tok1 != ID) return SYN_ERR; //nejaka chyba
					if(tok2 != VICE) return SYN_ERR;
					if(tok3 != ID) return SYN_ERR; //nejaka chyba
					vkladany1 = find_tstack(table, atr1);
					vkladany3 = find_tstack(table, atr3);
					if (vkladany1 == NULL || vkladany3 == NULL) return SYN_ERR;
					if (vkladany1->defined == 0) return NONINC_ERR;
					if (vkladany3->defined == 0) return NONINC_ERR;
					if ((vkladany1->typ == INT_V || vkladany1->typ == DOUBLE_V) && (vkladany3->typ == INT_V || vkladany3->typ == DOUBLE_V)) type_var = 1;
					else if (vkladany1->typ == STRING_V && vkladany3->typ == STRING_V) type_var = 1;
					else return STYPE_ERR;
					generateVariable(&atr2); //v atr2 je zarucene NULL
					inicializuj_data(&vkladany2);
					vkladany2->typ = type_var;
					vkladany2->defined = 1;
					insert_tstack(table, atr2, vkladany2);
					// vygenerovat instrukci, ktera provede vygenerovana = ID + ID -> vkladany2 = vkladany 3 == vkladany 1
					printf("GENEROVANA INSTRUKCE: %s = %s > %s\n", atr2, atr3, atr1);
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
popPrc(&tok1, p_result, &PSAlist); // rozhodout se co budu chtit vracet??
printf("posledni pop: %s\n", *p_result);
return IS_OK;
//navrat hodnoty na pop(PSAlist), tam bude ta promenna ve ktere je ulozeny vysledek vyrazu
//konec PSA
}


int prec_prevod(int* token, char** attrc){
	tSymbolPtr vkladany;
	switch(*token){
		case INT_V:;
			int tmp_i;
			//attr se prevede na int hodnotu
			tmp_i = atoi(*attrc);
			free(*attrc);
			generateVariable(attrc);
			inicializuj_data(&vkladany);
			vkladany->typ = INT_V;
			vkladany->defined = 1;
			vkladany->value.i=tmp_i;
			insert_tstack (table, *attrc, vkladany);
			//unikatni identifikator se zapise do attr	
			//zmeni se hodnota tokenu na ID
			*token = ID;
			return S_ID;
		case DOUBLE_V:;
			double tmp_d;
			//attr se prevede na int hodnotu
			tmp_d = atof(*attrc);
			free(*attrc);
			generateVariable(attrc);
			inicializuj_data(&vkladany);
			vkladany->typ = DOUBLE_V;
			vkladany->defined = 1;
			vkladany->value.d=tmp_d;
			insert_tstack (table, *attrc, vkladany);
			//unikatni identifikator se zapise do attr	
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
//int parse(struct symbol_table* table_hl, tList *list){
int parse(uk_uzel *GT){ 
  int result;
  global_table = *GT;/*Nastaveni tabulky*/
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
			hledany = najdi_v_tabulce(global_table, nazev);
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
				vloz_do_tabulky(&global_table, nazev, hledany);
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
			push_tstack(&table, NULL, 1);
			printf("GENEROVANA LABELU FUNKCE: %s\n", funkce_v->symbol);
			//vytvorit instrukci label a dat ji definici funkci
			//funkcni label by mel byt asi jiny nez klasicky, aby vytvoril automaticky novou uroven a natahnul si tabulku -> bude resit CALL!
			//vyskladat parametry do lokalni tabulky
			outcome = load_params(funkce_v->parametry);
			if (outcome != IS_OK) return outcome;
			outcome = slozeny();
			if (outcome != IS_OK) return outcome;
			funkce_v->tabulka = table->tabulka;
			print_koren_udu(funkce_v->tabulka);
			pop_tstack(&table);
			//label/znacka ukonceni funkce (pokud se na nej dojde, tak nastala chyba typu nenalezen return prikaz)
			printf("GENEROVANI CHYBOVEHO LABELU PRO FUNKCE \n");
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
	char* ziskany = NULL;
	tSymbolPtr p_ziskany = NULL;
	tSymbolPtr p_prirazeny = NULL;
	char *label1 = NULL;
	char *label2 = NULL;
	char *label3 = NULL;
	tSymbolPtr p_label = NULL;
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
			p_prirazeny = find_tstack(table, attr.str);
			if (p_prirazeny == NULL) return SEM_ERR;
			getToken
			if (token != PRIRAZENI) return SYN_ERR;			
			getToken
			outcome = prirazeni(&ziskany);
			if (outcome != IS_OK) return outcome;
			p_ziskany = find_tstack (table, ziskany);	//potrebujeme najit datovy typ promenne
			if ((p_ziskany->typ == STRING_V && p_prirazeny->typ != STRING_V) || (p_ziskany->typ != STRING_V && p_prirazeny->typ == STRING_V)) return STYPE_ERR;
			p_prirazeny->defined = 1;
			printf("GENEROVANA INSTRUKCE: %s = %s\n", p_prirazeny->symbol, p_ziskany->symbol);
			if (token != STREDNIK) return SYN_ERR;
			getToken
			return IS_OK;
			break;
		/* PRIKAZ → if lz expr pz SLOZENY else SLOZENY */
		case IF:
			getToken
			if (token != L_ZAVORKA) return SYN_ERR;
			getToken
			outcome = syntax_precedencka(&ziskany);
			if (outcome != IS_OK) return outcome;
			if (token != P_ZAVORKA) return SYN_ERR;
			p_ziskany = find_tstack (table, ziskany);
			if (!(p_ziskany->typ == INT_V || p_ziskany->typ == DOUBLE_V)) return STYPE_ERR;
			generateVariable(&label1);
			printf("GENEROVANA INSTRUKCE: JUMP ON 0 podminka: %s kam: %s\n", p_ziskany->symbol, label1);
			generateVariable(&label2);
			printf("GENEROVANA INSTRUKCE: LABEL WITH PUSH (work table) %s\n", label2);
			inicializuj_data(&p_label);
			//vloz adresu labelu do dat
			push_tstack(&table, NULL, 0);
			getToken
			outcome = slozeny();
			if (outcome != IS_OK) return outcome;
			p_label->tabulka = table->tabulka;
			print_koren_udu(p_label->tabulka);
			pop_tstack(&table);
			insert_tstack(table, label2, p_label);
			printf("GENEROVANA INSTRUKCE: POP TABLE\n");
			generateVariable(&label3);
			printf("GENEROVANA INSTRUKCE: JUMP %s\n", label3);
			p_label = NULL;
			if (token != ELSE) return SYN_ERR;
			printf("GENEROVANA INSTRUKCE: LABEL WITH PUSH (work table) %s\n", label1);
			inicializuj_data(&p_label);
			//vloz adresu labelu do dat
			push_tstack(&table, NULL, 0);
			getToken
			outcome = slozeny();
			if (outcome != IS_OK) return outcome;
			p_label->tabulka = table->tabulka;
			print_koren_udu(p_label->tabulka);
			pop_tstack(&table);
			insert_tstack(table, label1, p_label);
			printf("GENEROVANA INSTRUKCE: POP TABLE\n");
			printf("GENEROVANA INSTRUKCE: LABEL %s\n", label3);
			p_label = NULL;
			inicializuj_data(&p_label);
			//vloz adresu labelu do dat
			insert_tstack(table, label3, p_label);
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
			outcome = syntax_precedencka(&ziskany);
			if (outcome != IS_OK) return outcome;
			// expr dava nacteny dalsi token
			if (token != STREDNIK) return SYN_ERR;
			getToken
			if (token != ID) return SYN_ERR;
			getToken
			if (token != PRIRAZENI) return SYN_ERR;
			getToken
			// expr
			outcome = syntax_precedencka(&ziskany);
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
			outcome = syntax_precedencka(&ziskany);
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

int prirazeni(char **ziskany){	//case ID and id je jmeno funkce... na tomto nam bude padat kontrola
	int outcome;
	tSymbolPtr p_ziskany = NULL;


	switch(token){
		/* PRIRAZENI → id lz TERMY pz */
		case ID:
			if (find_tstack(table, attr.str) == NULL){
				p_ziskany = najdi_v_tabulce(global_table, attr.str);
				//dodelat obsluhu callu, tedy napushovat co je v termech a pak az volat CALL a za CALLem vytahnout do promenne vysledek
				if (p_ziskany == NULL) return SEM_ERR;
				getToken
				if (token != L_ZAVORKA) return SYN_ERR;
				getToken
				outcome = termy(p_ziskany->parametry);
				if (outcome != IS_OK) return outcome;
				printf("GENEROVANA INSTRUKCE: CALL %s\n", p_ziskany->symbol);
				generateVariable(ziskany);
				tSymbolPtr p_mezivysledek;
				inicializuj_data(&p_mezivysledek);
				p_mezivysledek->typ = p_ziskany->typ;
				p_mezivysledek->defined = 1;
				insert_tstack(table, *ziskany, p_mezivysledek);
				printf("GENEROVANA INSTRUKCE: POP INTO %s\n", *ziskany);
				if (token != P_ZAVORKA) return SYN_ERR;
				getToken
				return IS_OK;
			}
		// case expresiion <<< PRIRAZENI → expr
		default:
			outcome = syntax_precedencka(ziskany);
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
	char *ziskany;
	switch(token){
		/* VYPIS → pis TERM DVYPIS */
		case ZAPIS:
			getToken
			outcome = term(&ziskany);
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
	char* nazev = NULL;
	tSymbolPtr vkladany = NULL;
	tSymbolPtr p_ziskany = NULL;
	char* ziskany = NULL;
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/* PROMENNA → TYP id PRIRAD */
			outcome = typ(&typ_v);
			if (outcome != IS_OK) return outcome;
			if (token != ID) return SYN_ERR;
			string_from_char(&nazev, &attr);
			if (najdi_v_tabulce (table->tabulka, nazev) != NULL) return SEM_ERR;
			inicializuj_data(&vkladany); //alokuje pamet a inicializuje hodnoty
			vkladany->typ = typ_v;
			insert_tstack (table, nazev, vkladany);
			getToken
			outcome = prirad(&ziskany);
			if (outcome != IS_OK) return outcome;
			if (ziskany != NULL){	//pokud byl vyraz a ne strednik
				p_ziskany = find_tstack (table, ziskany);	//potrebujeme najit datovy typ promenne
				if ((p_ziskany->typ == STRING_V && vkladany->typ != STRING_V) || (p_ziskany->typ != STRING_V && vkladany->typ == STRING_V)) return STYPE_ERR;
				vkladany->defined = 1;
				printf("GENEROVANA INSTRUKCE: %s = %s\n", nazev, ziskany);	//je potreba zkontrolovat jen spravne prirazeni o vse ostatni se postaraji funkce drive
			}
			return IS_OK;
			break;
		case AUTO:
		 /* PROMENNA → auto id rovnitko expr */
			getToken
			if (token != ID) return SYN_ERR;
			string_from_char(&nazev, &attr);
			if (najdi_v_tabulce (table->tabulka, nazev) != NULL) return SEM_ERR;
			inicializuj_data(&vkladany); //alokuje pamet a inicializuje hodnoty
			vkladany->defined = 1;
			insert_tstack (table, nazev, vkladany);
			getToken
			if (token != PRIRAZENI) return AUTO_ERR;
			getToken
			outcome = syntax_precedencka(&ziskany);//DODELAT?
			if (outcome != IS_OK) return outcome;
			p_ziskany = find_tstack (table, ziskany);	//potrebujeme najit datovy typ promenne
			vkladany->typ = p_ziskany->typ;
			printf("typy: %i %i\n", p_ziskany->typ, vkladany->typ);
			printf("GENEROVANA INSTRUKCE: %s = %s\n", nazev, ziskany);
			return IS_OK;
			break;
		default:
		 	return SYN_ERR;
		 	break;
	}
}

int prirad(char **ziskany){
	int outcome;
	switch(token){
		case PRIRAZENI:
		/* PRIRAD → rovnitko expr */	
			getToken
			outcome = syntax_precedencka(ziskany);
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

int termy(param *parametry_f){
	int outcome;
	char* ziskany;
	tSymbolPtr p_ziskany = NULL;
	switch(token){
		/* TERMY → TERM DTERM */
		case ID:
		case INT_V:
		case DOUBLE_V:
		case STRING_V:
			if (parametry_f == NULL) return STYPE_ERR;
			outcome = term(&ziskany);
			if (outcome != IS_OK) return outcome;
			p_ziskany = find_tstack (table, ziskany);
			if (p_ziskany->typ != parametry_f->typ) return STYPE_ERR;
			printf("GENEROVANA INSTRUKCE: PUSH %s\n", ziskany);
			outcome = dterm(parametry_f->next);
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case P_ZAVORKA:
			if (parametry_f != NULL) return STYPE_ERR;
		/* TERMY → ε */
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int dterm(param *parametry_f){
	int outcome;
	char* ziskany;
	tSymbolPtr p_ziskany = NULL;
		/* DTERM → ε */
	switch(token){
		case P_ZAVORKA:
			if (parametry_f != NULL) return STYPE_ERR;
			return IS_OK;
			break;
		/* DTERM → carka TERM DTERM */
		case CARKA:
			getToken
			if (parametry_f == NULL) return STYPE_ERR;
			outcome = term(&ziskany);
			if (outcome != IS_OK) return outcome;
			p_ziskany = find_tstack (table, ziskany);
			if (p_ziskany->typ != parametry_f->typ) return STYPE_ERR;
			printf("GENEROVANA INSTRUKCE: PUSH %s\n", ziskany);
			outcome = dterm(parametry_f->next);
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int term(char **ziskany){
	switch(token){
		tSymbolPtr p_ziskany = NULL;
		/* TERM → id */
		case ID:
			p_ziskany = find_tstack (table, attr.str);
			if (p_ziskany->defined == 0) return SEM_ERR;
			*ziskany = p_ziskany->symbol;
			getToken
			return IS_OK;
			break;
		case INT_V:;
			int tmp_i;
			tmp_i = atoi(attr.str);
			generateVariable(ziskany);
			inicializuj_data(&p_ziskany);
			p_ziskany->typ = INT_V;
			p_ziskany->defined = 1;
			p_ziskany->value.i=tmp_i;
			insert_tstack (table, *ziskany, p_ziskany);
			//unikatni identifikator se zapise do attr	
			/* TERM → literal */
			getToken
			return IS_OK;
			break;
		case DOUBLE_V:;
			double tmp_d;
			//attr se prevede na int hodnotu
			tmp_d = atof(attr.str);
			generateVariable(ziskany);
			inicializuj_data(&p_ziskany);
			p_ziskany->typ = DOUBLE_V;
			p_ziskany->defined = 1;
			p_ziskany->value.d=tmp_d;
			insert_tstack (table, *ziskany, p_ziskany);
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

int load_params(param *parametry_v){
	tSymbolPtr vkladany;
	while(parametry_v != NULL){
		vkladany = NULL;
		if (find_tstack(table, parametry_v->name) != NULL) return SEM_ERR;
		inicializuj_data(&vkladany); //alokuje pamet a inicializuje hodnoty
		vkladany->typ = parametry_v->typ;
		vkladany->defined = 1;
		insert_tstack (table, parametry_v->name, vkladany);
		printf("GENEROVANA INSTRUKCE: PUSH INTO %s\n", parametry_v->name);
		parametry_v = parametry_v->next;
	}
	return IS_OK;
}