#include "define.h"
#include "stack.h"

typedef struct listPrecedence {
    struct listPrecedence *prev;
    int s_token;
    string promenna;
} *PrcPtr;	//11.11.2015 19:15 prc by Korgo
//dopsat inicializaci, pridani, odebrani

int pushPrc(int s_token, string promenna, PrcPtr list) {	//navratova hodnota funkce je jestli byla vnitrni chyba, nebo ne
	//todo
	//mozna pro terminal_list vytvorit variantu, kde se nepushuje promenna
}
void popPrc(int* s_token, string* promenna, PrcPtr list){
	//todo
}
void emptyPrc(PrcPtr list){
	//todo todo todooo
}
int topPrc(PrcPtr list){	//vraci hodnotu tokenu na topu listu
	//tudududu
	//jenom vrati hodnotu co je na topu, ale nepopuje ji, bude vyuzivat jen terminal list
}


PrcPtr PSAlist;	//Zasobnik vsech ulozenych prvku (pushdown zasobnik)
PrcPtr terminal_list;	//zasobik prvku, ktere jsou pro nas relevantni pro porovnavani posledniho znaku na zasobniku, (v prezentacich se za ne uklada "<")
int vysl_prc = 0;		//zde se uklada navratova hodnota z tabulky
int terminalTop = 13;	//hodnota/nazev terminalu na vrcholu zasobniku pro PSA
int nacteny = 13;		//hodnota/nazev nacitaneho tokenu

int Table [14][14] =
// sloupec - vstupni token, radek - znak na zasobniku, tabulka jeste neni opravena!!
{ 
//		   ID 	  (		  )       +        -      *       /       ==      <=       <      !=      >=      >       $
/*ID*/	{SHERR,	SHERR,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*(*/	{SHIFT,	SHIFT,	HANDL,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHERR},
/*)*/	{SHERR,	SHERR,	RPLCE,	RPLCE,	RPLCE	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
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
/*$*/	{SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	IS_OK}

};

//samotny kod PSA (precedencni SA)

getToken;	//asi nebude nutny, zalezi jestli ten token prvni za nas dostane LL nebo ne
//push na teminallist DOLAR 
nacteny = prec_prevod(&token, &attr);	//zase otazka jestli se ma pouzit dereference, nebo attr sam o sbe je ukazatel, pak se podivat a poresit na vsech mistech
do{
	vysl_prc = Table[terminalTop][nacteny];
	switch (vysl_prc){
		case SHIFT:
			pushPrc(token, attr, PSAlist);
			pushPrc(token, attr, terminal_list);
			getToken;
			nacteny = prec_prevod(&token, &attr);
			break;
		case HANDL:
			//vytahnout levou zavorku z terminal_list (pop(terminallist))
			//2x pop a nekam si ulozit hodnoty z PSAlist
			//zkontrolovat jestli odpovida tvar: "(ID)", pokud ne vratit spravny error
			//pokud tvar odpovida, pushnout ID na PSAlist
			getToken;
			nacteny = prec_prevod(&token, &attr);			
			break;
		case RPLCE:
			switch(terminalTop){
				case ID:
					//pop terminal list, nic vic. Pro nas je to jak kdyby prevod ID -> E
					break;
				case PLUS:
					// pop(terminallist) -> zpracovali jsme terminal na topu
					// 3x pop PSAlist a kazdy pop si ulozit, nezapomenout ze se to popuje od zadu!!! a-b vs b-a
					// zkotrolovat jestli odpovida tvar ID + ID, pokud ne vyhodit error (napr mohlo by prijit + ID +, v pripade ze by se redukce provedla pred zadanim promenne)
					// pokud vse odpovida:
					// vygenerovat promennou do ktere se ulozi vysledek
					// vygenerovat instrukci ktera provede vygenerovana = ID + ID
					// pushnout do PSAlist vygenerovanou promennout (tu do ktere se ulozil vysledek)
					break;
				case MINUS:
					//ta sama opici prace jak vyse, akorat s jinym znaminkem, a bude to pro vsechny operace, ktere nasleduji
					//mozna by ten kod sel zefektivnit, ze case bude potom az na kazde generovani instrukci, avsak takto je to bezpecny a lehceji se dohledaji chyby
					break;
				case NASOBENI:
					break;
				case DELENI:
					break;
				case ROVNA_SE:
					break;
				case MENE_NEBO_ROVNO:
					break;
				case MENE:
					break;
				case NEROVNOST:
					break;
				case VICE_NEBO_ROVNO:
					break;
				case VICE:
					break;
				default:
					//stdout ze tady by to nikdy dojit nemelo a najit v jakem pripade se to stalo :)
					break;
			}
			break;
		case SHERR:
			return ERR; //provest cisteni a jeste vyresit jaka chyba se posle!
	}

	terminalTop = topPrc(terminal_list);
}while (nacteny != DOLAR || terminalTop != DOLAR)
//navrat hodnoty na pop(PSAlist), tam bude ta promenna ve ktere je ulozeny vysledek vyrazu

//konec PSA



int prec_prevod(int* token, string* attr){ //ma to byt ukazatel, nebo string sam o sobe je ukazatel na strukturu?
	switch(token){
		case INT_V:
			//attr se prevede na int hodnotu
			//hodnota se ulozi s unikatnim identifikatorem do lokalni tabulky (+ dalsi dulezite informace, jako ze je to int, ze je to promenna atd...)
			//unikatni identifikator se zapise do attr
			//zmeni se hodnota tokenu na ID
			return 0;
		case DOUBLE_V:
			//attr se prevede na double hodnotu
			//hodnota se ulozi s unikatnim identifikatorem do lokalni tabulky
			//unikatni identifikator se zapise do attr
			//zmeni se hodnota tokenu na ID
			return 0;
		case STRING_V:
			//attr se zkopiruje na novy oddil pameti (kdyby se zkopirval ukazatel, tak po zmene attr, by jsme ztratili "hodnotu")
			//hodnota se ulozi s unikatnim identifikatorem do lokalni tabulky
			//unikatni identifikator se zapise do attr
			//zmeni se hodnota tokenu na ID
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