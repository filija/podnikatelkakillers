#include <stdio.h>
#include <stdlib.h>

#define MAXSTACK 30;

//zasobnik obsahuju data tabulky symbolu
typedef struct zasobnikPolozek{
	tData *data;
	cleny clen;
} pStack;

typedef struct zasobnikHodnot{
	zasobnikPolozek *zP;
	int vrchol;
	int velikost;
} zStack;

int stackInit(zStack *stack){

	//incializace hlavniho zasobniku
	if ( (*stack = malloc(sizeof(struct zasobnikHodnot))) == NULL ){
		return INTERNAL_ERR;
	}

	//do ukazatele hlavniho zasobniku alokujeme 30x prostor pro zasobnikovou polozku
	if ( (*stack->zP = malloc(MAXSTACK * sizeof(struct zasobnikHodnot))) == NULL)
		return INTERNAL_ERR;

	//nainicializovani vnitrnich zasobniku na hodnoty 0/NULL
	for (int i = 0; i < MAXSTACK; ++i){
		*stack->data = NULL;
		*stack->clen = 0;
	}

	*stack->vrchol = 0;
	*stack->velikost = MAXSTACK;

	return 0;	
}

void stackPush(zStack *stack, tData *newData, int newClen){
	if (*stack->vrchol == *stack->velikost){
		*stack->velikost = MAXSTACK + *stack->velikost;
		*stack->zP = realloc (*stack->zP, *stack->velikost * sizeof(sturct zasobnikHodnot));
	}
	*stack->vrchol++;
	*stack->zP[*stack->vrchol].clen = newClen;
	*stack->zP[*stack->vrchol].data = newData;
}

void stackPop(zStack *stack){
	if (*stack->vrchol > 0){
		*stack->zP[*stack->vrchol].clen = 0;
		*stack->zP[*stack->vrchol].data = NULL;
		*stack->vrchol--;
	}
}

int stackTop(zStack *stack){
	if (*stack->vrchol == 0)
		return INTERNAL_ERR;
	return *stack->zp[*stack->vrchol];
}

void stackDispose(zStack *stack){
	free (*stack->zP);
	free (*stack);
}