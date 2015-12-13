/*
*	IFJ Projekt
*		scanner pro jazyk IFJ15
*		autor: Jakub Korgo, Martin Kruták
*		login: xkorgo01, xkruta03
*/


#include "tabulka_symbolu.h"

int push_tstack(tStackPtr *stack, uk_uzel tabulka, int zarazka){
	if (*stack == NULL){
		*stack =  malloc(sizeof(struct ZasobnikTabulek));
		if (*stack != NULL){
			(*stack)->tabulka = tabulka;
			(*stack)->zarazka = zarazka;
			(*stack)->next = NULL;
			return IS_OK;
		}
		return INTERNAL_ERR;
	}
	tStackPtr tmpstack = malloc(sizeof(struct ZasobnikTabulek));
	if (tmpstack!=NULL){
		tmpstack->tabulka = tabulka;
		tmpstack->zarazka = zarazka;
		tmpstack->next = *stack;
		*stack = tmpstack;
		return IS_OK;
	}
	return INTERNAL_ERR;
}
int pop_tstack(tStackPtr *stack){
	if (*stack == NULL) return INTERNAL_ERR;
	tStackPtr tmpstack;
	tmpstack = (*stack)->next;
	free(*stack);
	*stack = tmpstack;
	return IS_OK;
}

tSymbolPtr find_tstack(tStackPtr stack, char* name){
	tSymbolPtr result;
	while (stack != NULL){
		result = najdi_v_tabulce(stack->tabulka, name);
		if (result != NULL) return result;
		else{
			if (stack->zarazka == 1) return NULL;
				stack = stack->next;
		}
	}
	return NULL;
}

tSymbolPtr out_find_tstack(tStackPtr stack, char* name){
	tStackPtr tmpstack;
	tSymbolPtr tmpres;
	tSymbolPtr result;
	while (stack != NULL){
		result = najdi_v_tabulce(stack->tabulka, name);
		if (result != NULL) return result;
		tmpstack = stack->next;
		result = najdi_v_tabulce(tmpstack->tabulka, name);
		if (result != NULL){
			copy_item(&tmpres, result);
			vloz_do_tabulky(&(stack->tabulka), name, tmpres);
			return tmpres;
		} 
		else{
			if (stack->zarazka == 1) return NULL;
			stack = tmpstack->next;
		}
	}
	return NULL;
}

int insert_tstack(tStackPtr stack, char *K, tSymbolPtr data){
	if (stack == NULL) return INTERNAL_ERR;
	return vloz_do_tabulky(&(stack->tabulka), K, data);
}

int check_defined(uk_uzel glob_table){
	int output;
	if(glob_table == NULL) return IS_OK;
	if(glob_table->data->defined == 0) return SEM_ERR;
	output = check_defined(glob_table->LPtr);
	if (output != IS_OK) return output;
	output = check_defined(glob_table->RPtr);
	if (output != IS_OK) return output;
	return IS_OK;
}

void znic_pracovni_tabulku(uk_uzel Koren) {	//zruseni storomu
	if(Koren == NULL){
		return;
	}
	znic_pracovni_tabulku((Koren)->LPtr); // jedem vlevo
	znic_pracovni_tabulku((Koren)->RPtr);	// jedem vpravo
	if ((Koren->data)->typ == 3) free((Koren->data)->value.s);
	free(Koren->data);
	free(Koren);
	Koren = NULL;
}