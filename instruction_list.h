#ifndef INSTRUCTION_LIST_H
#define INSTRUCTION_LIST_H
#include "define.h"
#include <stdlib.h>

#define I_SCITANI		1
#define I_ODCITANI		2
#define I_NASOBENI		3
#define I_DELENI		4
#define I_MENSI			5
#define I_MENSI_NEBO	6
#define I_VETSI			7
#define I_VETSI_NEBO	8
#define I_ROVNO			9
#define I_NEROVNO		10
#define I_PRIRAZENI		11
#define I_SKOK			12
#define I_PODM_SKOK		13	//skok pokud neplati!
#define I_LABEL			14
#define I_CTENI			15
#define I_ZAPIS			16
#define I_LABEL_LOAD	17
#define I_UNLOAD		18
#define I_RETURN		19
#define I_FUN_ERR		20
#define I_PUSH			21
#define I_POP			22
#define I_CALL			23


typedef struct instructionlist{
  int instType;  // typ instrukce
  void *addr1; // adresa 1
  void *addr2; // adresa 2
  void *addr3; // adresa 3
  struct instructionlist *next;
} tInstr;

typedef struct
{
  tInstr *first;  // ukazatel na prvni prvek
  tInstr *last;   // ukazatel na posledni prvek
  tInstr *active; // ukazatel na aktivni prvek
} tListOfInstr;

void listInit(tListOfInstr *L);
int listInsertLast(tListOfInstr *L, int instType, void *addr1, void *addr2, void *addr3);
void listFirst(tListOfInstr *L);
void listNext(tListOfInstr *L);
void *listGetPointerLast(tListOfInstr *L);
void listGoto(tListOfInstr *L, void *gotoInstr);

#endif /* INSTRUCTION_LIST_H */