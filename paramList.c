#include <stdio.h>
#include <stdlib.h>
#include "define.h"

/*
* listItem -> struktura obsahujici informace o polozce seznamu
* next -> ukazatel na nasledujici prvek
* datatypes: INT_V(1), DOUBLE_V(2), STRING_V(3)
*/

//polozka seznamu obsahujici ifnromaci o datovem typu a hodnotu promenne
typedef struct listItem {
    struct listItem *next;
    int datatype;
} *itemPtr;

//list obsahujici ukazatele na prvni a aktualni polozku seznamu
typedef struct { 
    itemPtr First;
    itemPtr Act;
} list;

//inicializace seznamu
void listInit(list *L){
	L->First = NULL;
	L->Act = NULL;
}

//pridani noveho prvku na konec seznamu
void listInsert(list *L, int datatype){
	
	itemPtr tmpItem = (itemPtr)malloc( sizeof(struct listItem) ); //alokace pameti pro novou strukturu
	
	tmpItem->next = NULL; //bude posledni, tedy next bude null
	tmpItem->datatype = datatype; //nastaveni hodnot nove struktury

	L->Act = L->First; //aktualni prvek bude prvni
	if (L->First != NULL){
		while (L->Act->next != NULL) {//dojdi na konec seznamu
			L->Act = L->Act->next;
		}
		L->Act->next = tmpItem;
	}
	else
		L->First = tmpItem;
}

//uvolneni pameti celeho seznamu
void listDispose(list *L){

	itemPtr Next = NULL; //ukazatel na dalsí prvek
	while(L->First != NULL){ //dokud není prvni polozka null, tak si zapise ukazatel na prvni položku a nultou smaže, následně první nastaví jako nultou
		Next = L->First->next;
		free(L->First);
		L->First = Next;
	}

	L->Act = NULL;

}

void listEcho(list *L){
	printf("\n-----START ECHO-----\n");
	L->Act = L->First;
	int i = 0;
	while(L->Act != NULL){
		printf("Item n.%i has datatype: %i\n", i, L->Act->datatype);
		L->Act = L->Act->next;
		i++;
	}
	printf("------END ECHO------\n");
}

int main(){
	list paramList;
	listInit(&paramList);

	listInsert(&paramList, STRING_V);
	listInsert(&paramList, INT_V);
	listInsert(&paramList, DOUBLE_V);
	listInsert(&paramList, STRING_V);

	listEcho(&paramList);
	listDispose(&paramList);
}