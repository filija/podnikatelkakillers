#include <stdio.h>
#include "str.h"
#include "stable.h"
#include "ilist.h"
#include "scaner.h"
#include "parser.h"
#include "define.h"

int token;
int countVar = 0;
struct symbol_table* table;
// ptrStack stack;
string attr;
string *params;
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
			if ((outcome = getNextToken(&attr) == LEX_ERR))
			{
				return LEX_ERR;
			}
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
			if ((outcome = getNextToken(&attr) == LEX_ERR))
			{
				return LEX_ERR;
			}
			switch(outcome){
				case STREDNIK:
					return IS_OK;
				case LS_ZAVORKA:
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
	switch(token){
		//FUNKCEZ → PARAMETR lz PARAMETRY pz

	}
}






















