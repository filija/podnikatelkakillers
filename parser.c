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
		/*PROGRAM → FUNKCES eof*/
		case INT:
		case DOUBLE:
		case STRING:
			outcome = funkces();
			if (outcome != IS_OK)
			{
				return outcome;
			}
			getToken
			if (outcome != EOF)
			{
				return SYN_ERR;
			}
			return IS_OK;
			break;
		default:
			return SEM_ERR;
			break;
	}
}

int funkces(){ // reseni i pro eps pravidlo 3
	int outcome;
	switch(token){
		// FUNKCES → FUNKCE FUNKCES
		case INT:
		case DOUBLE:
		case STRING:
			outcome = funkce();
			if (outcome != IS_OK)
			{
				return outcome;
			}
			outcome = funkces();
			if (outcome != IS_OK)
			{
				return outcome;
			}

			return IS_OK;
			break;
		default: // nenastane, jen z principu or eps
			return IS_OK;
			break;
	}
}

int funkce(){
	int outcome;
	switch(token){
		// FUNKCE → FUNKCEZ SLOZENY
		// FUNKCE → FUNKCEZ strednik
		// FUNKCE → eps
		case INT:
		case DOUBLE:
		case STRING:
			outcome = funkcez();
			if (outcome != IS_OK)
			{
				return outcome;
			}
			getToken
			switch(outcome){
				case STREDNIK:
					// vlozeni do glob_tabSym
					return IS_OK;
				case LS_ZAVORKA:
					// vlozeni do glob_tabSym
					outcome = slozeny();
					if (outcome != IS_OK)
					{
						return outcome;
					}
					break;
				default:
			}
			break;
		default:
			return IS_OK;
			break;
	}
}

int funkcez(){
	int outcome;
	string copyatr;
	switch(token){
		//FUNKCEZ → PARAMETR lz PARAMETRY pz
		case INT:
			data->typ = INT_V;
			break;
		case DOUBLE:
			data->typ = DOUBLE_V;
			break;
		case STRING:
			data->typ = STRING_V;
			break;
		default:
	}	
	getToken
	if (token != ID)
	{
		return SYN_ERR;
	}else{
		if((strCopyString(copyatr, attr) == STR_ERROR){
			return INTERNAL_ERR;
		}
	}
	getToken
	if (token != L_ZAVORKA)
	{
		return SYN_ERR;
	}
	getToken
	switch(token){
		case INT:
		case DOUBLE:
		case STRING:
		 	outcome = parametry();
		 	if (outcome != IS_OK)
		 	{
		 		return outcome;
		 	}
		 	break;
		case P_ZAVORKA:
			return IS_OK;
		default:
			return SYN_ERR;
			break;
	}
}

int parametry(){
	int outcome;
	switch(token){
		//PARAMETRY → PARAMETR DPARAMETR (parametry)
		case INT:
			// ulozeni hodnoty
			break;
		case DOUBLE:
			// sameAs
			break;
		case STRING:
			//sameAs
			break;
		default:
			return SYN_ERR;
			break;
	}
	getToken
	if (token != ID)
	{
		return SYN_ERR;
	} 
	// ulozeni ID do tabsym
	getToken 
	switch(token){
		case CARKA:
			getToken 
			outcome = parametry();
			if (outcome != IS_OK)
			{
				return outcome;
			}
			break;
		case P_ZAVORKA:
			return IS_OK;
			break;
		default:
			return SYN_ERR;
	}
	return IS_OK;
}

int slozeny(){
	int outcome;
	getToken 
	switch(token){
		//SLOZENY → lsz PRIKAZY psz
		case PS_ZAVORKA:
			return IS_OK;
			break; //kvuli Korgymu a KAntymu
		//PRIKAZY → PRIKAZ PRIKAZY
		case ID:
		case IF:
		case FOR:
		case RETURN:
		case CIN:
		case COUT:
		case AUTO:
		case INT:
		case DOUBLE:
		case STRING:
			outcome = prikaz();
			if (outcome != IS_OK)
			{
				return outcome;
			}
			if (token == PS_ZAVORKA)
			{
				return IS_OK;
			}else{
				return SYN_ERR;
			}
			break;
		default:
			return SYN_ERR;		
	}
}

int prikaz(){
	int outcome;
	getToken
	//PRIKAZY → eps
	//PRIKAZ → PROMENNA	
	//PRIKAZ → id prirazeni expr strednik	
	//PRIKAZ → id prirazeni id lz TERMY pz strednik
	//PRIKAZ → if lz expr pz SLOZENY else SLOZENY	
	//PRIKAZ → for lz PROMENNA strednik expr strednik id prirazeni expr pz SLOZENY	
	//PRIKAZ → return expr strednik	
	//PRIKAZ → cin NACTENI strednik	
	//PRIKAZ → cout VYPIS strednik 
	switch(token){
		case PS_ZAVORKA:
			return IS_OK;
			break;
		case AUTO:
		case INT:
		case DOUBLE:
		case STRING:
			outcome = def_promenna();
			if (outcome != IS_OK)
			{
				return outcome;
			}
			break;
		case ID:
			hledane_id = najdi_v_tabulce(table,attr.str);
			if (hledane_id == NULL) // najdi v tabulce
			{
				return SEM_ERR;
			}
			if ((hledane_id->defined) == 0)
			{
				return SEM_ERR;
			}			
			getToken 
			if (token != PRIRAZENI)
			{
				return SYN_ERR;
			}
			getToken
			switch(token){
				//case expr: zasobnik expr - dodelat!! >>>>        <<<<<
				case ID:
					hledane_id = najdi_v_tabulce(table,attr.str);
					if (hledane_id == NULL) // najdi v tabulce
					{
						return SEM_ERR;
					}
					if ((hledane_id->defined) == 0)
					{
						return SEM_ERR;
					}
					if (hledane_id->verze) // funkce pro hodnotu 1
					{
						getToken
						if (token != L_ZAVORKA)
						{
							return SYN_ERR;
						}
						// nacteni vsech parametru
						getToken
						if (token != P_ZAVORKA)
						{
							return SYN_ERR;
						}
						getToken
						if (token != STREDNIK)
						{
							return SYN_ERR;
						}
						return IS_OK; //naprosto citelne
						
					}
					// reseni vyrazu
					getToken
					if (token != STREDNIK)
					{
						return SYN_ERR;
					}
					return IS_OK;		
					// zasobniky pro precedenci			
			}


	}
}
