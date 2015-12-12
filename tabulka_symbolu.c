#include "tabulka_symbolu.h"
/*
* Inicializuje koren tabulky
@par1 odkaz na koren (&koren)
*/
void inicializuj_tabulku(uk_uzel *Koren) { // inicializace stromu

	*Koren = NULL;
}

int inicializuj_data(tSymbolPtr *symbol){
	tSymbolPtr help_ptr = malloc(sizeof(struct sym));
	if (help_ptr == NULL) return INTERNAL_ERR;
	help_ptr->symbol = NULL;
	help_ptr->typ = 0;
	help_ptr->verze = 0;
	help_ptr->parametry = NULL;
	help_ptr->value.s = NULL;
	help_ptr->defined = 0;
	help_ptr->tabulka = NULL;
	*symbol = help_ptr;
	return IS_OK;
}

/*
Funkce hleda dle zadaneho klice - symbol, zaznam v tabulce
@par1 koren
@par2 klic
@return - v pripade nalezu vraci hodnotu klicem zadane polozky pokud nenalezne vraci 0
*/

tSymbolPtr najdi_v_tabulce(uk_uzel Koren, char *K)	{ // vyhledani uzlu zadaneho klicem

	if(Koren == NULL){
		return NULL;									// prazdny strom
	}
	if(strcmp(Koren->symbol,K) == 0){
		return Koren->data;
	}
	if(strcmp(Koren->symbol,K) > 0){
		return najdi_v_tabulce(Koren->LPtr, K);	// klic menci nez zadany - jedeme vlevo
	}else{
		return najdi_v_tabulce(Koren->RPtr, K);	// jinak vrpavo a znova
	}
}
/*
Funkce vlozi do tabulky zaznam
@par1 koren
@par2 klic - symbol (nazev promenne)
@par3 verze - promenna, funkce
@return - v pripade nealokovani noveho korene - nic
*/

int vloz_do_tabulky(uk_uzel *Koren, char *K, tSymbolPtr data)	{	// vlozi data s klicem, nekontroluje se

	if ( *Koren != NULL ){
		if (strcmp((*Koren)->symbol,K) > 0)
			vloz_do_tabulky(&((*Koren)->LPtr), K, data);
		else if (strcmp((*Koren)->symbol,K) < 0)
			vloz_do_tabulky(&((*Koren)->RPtr), K, data);
		else
			return 1;
	}else{
		uk_uzel help_ptr;
		help_ptr = malloc(sizeof(struct SymbolTable));
		if ( help_ptr == NULL ){
			return INTERNAL_ERR;
		}
		help_ptr->symbol = K;
		data->symbol = K;
		help_ptr->data = data;
		help_ptr->LPtr = NULL;
		help_ptr->RPtr = NULL;
		*Koren = help_ptr;
	}
	return IS_OK;
}
/*
Funkce zrusi tabulku
@par1 koren
@return - nic
*/

void znic_tabulku(uk_uzel Koren) {	//zruseni storomu

	if(Koren == NULL){
		return;
	}
	znic_tabulku((Koren)->LPtr); // jedem vlevo
	znic_tabulku((Koren)->RPtr);	// jedem vpravo
	free(Koren->symbol);
//	free((Koren->data)->symbol);
	//free(Koren->data->par_typy); // toto bude slozitejsi
	free(Koren->data);
	free(Koren);
	Koren = NULL;
}

/*
* Funkce kontroluje na konci programu vsechny funkce na jejich definici
* @par1 Koren tabulky
* @return exit s chybou 3


void check(uk_uzel Koren)	{ // vyhledani uzlu zadaneho klicem

	if(Koren == NULL){
		return;									// prazdny strom
	}else{
		if((Koren->data.verze)==1){
			if(Koren->data.defined == 0) exit (3);
		}
	}

		check(Koren->LPtr);	// klic mensi nez zadany - jedeme vlevo
		check(Koren->RPtr);	// jinak vpravo a znova
}*/

/*
* Funkce provede deep copy struktury dat v tabulce symbolu
* @param cilovy ukazatel na strukturu
* @param zdrojovy ukazatel na strukturu dat
* @return potvrzeni uspesnosti nebo vnitrni chybu
*/
int copy_item(tSymbolPtr* dest, tSymbolPtr source){ //nebude potrebovat kopirovat parametry fce, bude vzdy se kopirovat jen promenna!
	tSymbolPtr ukazatel;
	ukazatel = malloc(sizeof(struct sym));
	if(ukazatel == NULL){
		return INTERNAL_ERR; // right code?
	}
	ukazatel->symbol = source->symbol;
	ukazatel->typ = source->typ;
	ukazatel->verze = source->verze;
	ukazatel->value = source->value;
	ukazatel->parametry = NULL;
	ukazatel->defined = source->defined;
	ukazatel->tabulka = NULL;
	ukazatel->label = NULL;
	*dest = ukazatel;
	return IS_OK;
}

int vloz_do_parametru(int typ, char* nazev, param **parametry_v){
	if (*parametry_v == NULL){		
		*parametry_v = malloc(sizeof(struct sym_param));
		if(*parametry_v == NULL) return INTERNAL_ERR;
		(*parametry_v)->typ = typ;
		(*parametry_v)->name = nazev;
		(*parametry_v)->next = NULL;
		return IS_OK;
	}
	param* tmpparam = *parametry_v;
	while (tmpparam->next != NULL){
		tmpparam = tmpparam->next;
	}
	param* tmpparam2 = malloc(sizeof(struct sym_param));
	if(tmpparam2 == NULL) return INTERNAL_ERR;
	tmpparam2->typ = typ;
	tmpparam2->name = nazev;
	tmpparam2->next = NULL;
	tmpparam->next = tmpparam2;
	return IS_OK;
}

int zkontroluj_parametry(param *parametr, tSymbolPtr data){
	if (parametr == NULL){
		if (data->parametry == NULL) return IS_OK;
		else return SEM_ERR;
	}
	if (data->parametry == NULL) return SEM_ERR;
	param *tmp = data->parametry;
	while (1){
		if ((tmp->typ)!=(parametr->typ)) return SEM_ERR;
		if (strcmp (tmp->name, parametr->name)) return SEM_ERR;
		if (parametr->next == NULL && tmp->next == NULL) return IS_OK;
		if (parametr->next == NULL || tmp->next == NULL) return SEM_ERR;
		parametr = parametr->next;
		tmp=tmp->next;
	}
}

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