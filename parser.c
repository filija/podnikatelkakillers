#include <stdio.h>
#include "str.h"
#include "stable.h"
#include "ilist.h"
#include "scaner.h"
#include "parser.h"
#include "define.h"
#include "tabulka_symbolu.h"

#define getToken if ((token = getNextToken(&attr)) == LEX_ERR){return LEX_ERR;}

int token;
int countVar = 0;
struct symbol_table *table;
struct symbol_table *glob_table;
// ptrStack stack;
string attr;
string *params;
tSymbolPtr hledane_id;
tSymbolPtr data; // data prubezna

tList *List; // list instrukci

void generateVariable(string *var)

{
  strClear(var);
  strAddChar(var, '$');
  int i;
  i = counterVar;
  while (i != 0)
  {
    strAddChar(var, (char)(i % 10 + '0'));
    i = i / 10;
  }
  countVar ++;
}

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


int Table [14][14] =
// radek- vstupni token, sloupec- znak na zasobniku
{ 
//		   ID 	  (		  )       +        -      *       /       ==      <=       <      !=      >=      >       $
/*ID*/	{RPLCE,	SHERR,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE,	RPLCE},
/*(*/	{SHIFT,	SHIFT,	HADLE,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHERR},
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
/*$*/	{SHIFT,	SHIFT,	SHERR,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHIFT,	SHERR}

};


/*Hlavni funkce parseru*/
int parse(struct symbol_table* table_hl, tList *list, ptrStack stack_hl){
  
  int result;
  table = table_hl;/*Nastaveni tabulky*/
  Stack = stack_hl;/*Nastaveni zasobniku*/
  strInit(&attr);
  
  LS = list;/*Nastaveni listu*/
  
  /*Kontrola prvniho tokenu*/
  if ((token = getNextToken(&attr)) == LEX_ERROR)
     result = LEX_ERROR;
  else
     result = program();/*Zavolani kontoly syntaxe programu*/
  
  
  /*Uvolneni retezcu*/
  strFree(&attr);
  if (str_parameters != NULL){
    strFree(str_parameters);
    free(str_parameters);
    }	
  
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
		case EOF:
			/* PROGRAM → DFUNKCE eof */
			outcome = dfunkce();
			if (outcome != IS_OK) return outcome;
			if (token != EOF) return SYN_ERR;
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
			/* DFUNKCE → FUNKCE DFUNKCE */
			outcome = funkce();
			if (outcome != IS_OK) return outcome;
			outcome = dfunkce();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case EOF:
			/* DFUNKCE → ε */
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}
}

int funkce(){
		/*<FUNKCE>*/
	int outcome;
	
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/* FUNKCE → DEKLARACE DEFINICE */
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
			/* DEKLARACE → PARAMETR lz PARAMETRY pz */
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

int parametr(){

	int outcome;
	
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/* PARAMETR → TYP id */
			outcome = typ();
			if (outcome != IS_OK) return outcome;
			if (token != ID) return SYN_ERR;
			//tady bude napriklad kod na pushuti ID do dat ktere se budou potom vkladat do tabulky
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}		
}

int typ(){

	int outcome;
	
	switch(token){
		case INT:
			/*TYP → int*/
			//zpracovani toho ze typ je INT (token nam zkontroluje switch, kterym do teto funkce lezeme)
			getToken
			return IS_OK;
			break;
		case DOUBLE:
			/*TYP → double*/
			// ----||----
			getToken
			return IS_OK;
			break;
		case STRING:
			/*TYP → string*/
			// ----||----
			getToken
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}		

}

int parametry(){
	int outcome;
	
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
			/*PARAMETRY → PARAMETR DPARAMETR*/
			outcome = parametr();
			if (outcome != IS_OK) return outcome;
			outcome = dparametr();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case P_ZAVORKA:
			/*PARAMETRY → ε*/
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}	
}

int dparametr(){
	int outcome;
	
	switch(token){
		case CARKA:
			/*DPARAMETR → carka PARAMETR DPARAMETR*/
			//overeni tohoto tokenu za nas dela switch
			getToken
			outcome = parametr();
			if (outcome != IS_OK) return outcome;
			outcome = dparametr();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case P_ZAVORKA:
			/*DPARAMETR → ε*/
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}	
}

int definice(){
	int outcome;
	
	switch(token){
		case STREDNIK:
			/*DEFINICE → strednik*/
			getToken
			return IS_OK;
			break;
		case LS_ZAVORKA:
			/*DEFINICE → SLOZENY*/
			outcome = slozeny();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}	
}

int slozeny(){
	int outcome;

	switch(token){
		case LS_ZAVORKA:
			/*SLOZENY → lsz PRIKAZY psz*/
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
			/*PRIKAZY → PRIKAZ PRIKAZY*/
			outcome = prikaz();
			if (outcome != IS_OK) return outcome;
			outcome = prikazy();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case PS_ZAVORKA:
			/*PRIKAZY → ε*/
			return IS_OK;
			break;
		default:
			return SYN_ERR;
			break;
	}

}

int prikaz(){
	int outcome;
	//DODELAT!!!!
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
		default:
			return SYN_ERR;
			break;
	}
}