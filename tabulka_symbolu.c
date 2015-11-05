#include "tabulka_symbolu.h"
/*
* Inicializuje koren tabulky
@par1 odkaz na koren (&koren)
*/
void inicializuj_tabulku(uk_uzel Koren) { // inicializace stromu

	Koren = NULL;
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
		return &(Koren->data);
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

int vloz_do_tabulky(uk_uzel Koren, char *K, tSymbol data)	{	// vlozi data s klicem

	if ( Koren != NULL ){
		if (strcmp((Koren)->symbol,K) > 0)
			vloz_do_tabulky(((Koren)->LPtr), K, data);
		else if (strcmp((Koren)->symbol,K) < 0)
			vloz_do_tabulky(((Koren)->RPtr), K, data);
		else
			return 1;
	}else{
		uk_uzel help_ptr;
		help_ptr = malloc(sizeof(struct SymbolTable));
		if ( help_ptr == NULL ){
			exit(99);
		}
		help_ptr->symbol = K;
		help_ptr->data = data;
		help_ptr->LPtr = NULL;
		help_ptr->RPtr = NULL;
		Koren = help_ptr;
	}
	return 0;
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
	free(Koren);
	Koren = NULL;
}

/*
* Funkce kontroluje na konci programu vsechny funkce na jejich definici
* @par1 Koren tabulky
* @return exit s chybou 3
*/

void check(uk_uzel Koren)	{ // vyhledani uzlu zadaneho klicem

	if(Koren == NULL){
		return;									// prazdny strom
	}else{
		if((Koren->data.verze)==1){
			if(Koren->data.defined == 0) exit (3);
		}
	}

		check(Koren->LPtr);	// klic menci nez zadany - jedeme vlevo
		check(Koren->RPtr);	// jinak vrpavo a znova
}
