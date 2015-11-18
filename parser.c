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
			/*PROGRAM → DFUNKCE eof*/
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
			/*DFUNKCE → FUNKCE DFUNKCE*/
			outcome = funkce();
			if (outcome != IS_OK) return outcome;
			outcome = dfunkce();
			if (outcome != IS_OK) return outcome;
			return IS_OK;
			break;
		case EOF:
			/*DFUNKCE → ε*/
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
	int outcome;

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
			if (token != ROVNITKO) return SYN_ERR;
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
	int outcome;
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
	int outcome;
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