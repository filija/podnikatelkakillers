#include "instruction_list.h"

void listInit(tListOfInstr *L)
// funkce inicializuje seznam instrukci
{
  L->first  = NULL;
  L->last   = NULL;
  L->active = NULL;
}

int listInsertLast(tListOfInstr *L, int instType, void *addr1, void *addr2, void *addr3)
// vlozi novou instruci na konec seznamu
{
	tInstr *newItem;
	newItem = malloc(sizeof (tInstr));
	if (newItem == NULL) return INTERNAL_ERR;
	newItem->instType = instType;
	newItem->addr1 = addr1;
	newItem->addr2 = addr2;
	newItem->addr3 = addr3;
	newItem->next = NULL;
	if (L->first == NULL) L->first = newItem;
	else L->last->next = newItem;
	L->last=newItem;
	return IS_OK;
}

void listFirst(tListOfInstr *L)
// zaktivuje prvni instrukci
{
  L->active = L->first;
}

void listNext(tListOfInstr *L)
// aktivni instrukci se stane nasledujici instrukce
{
  if (L->active != NULL)
  L->active = L->active->next;
}

void *listGetPointerLast(tListOfInstr *L)
// vrati ukazatel na posledni instrukci
{
  return (void*) L->last;
}

void listGoto(tListOfInstr *L, void *gotoInstr)
// nastavime aktivni instrukci podle zadaneho ukazatele
{
  L->active = (tInstr*) gotoInstr;
}

