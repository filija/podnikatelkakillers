/*
*	IFJ Projekt
*		scanner pro jazyk IFJ15
*		autor: Martin Kruták
*		login: xkruta03
*/


/*
* Implementace Shell-Sortu, Boyer-Mooreuv algortimus vyhledavani, tabulky za pomoci vyhledavaciho stromu a obsluznim rutinam kolem tabulek
*/

#include "ial.h"

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
	if ((Koren->data)->typ == 3) free((Koren->data)->value.s);
	if ((Koren->data)->parametry != NULL) znic_parametry((Koren->data)->parametry);
	if ((Koren->data)->tabulka != NULL) znic_tabulku((Koren->data)->tabulka);
	free(Koren->data);
	free(Koren);
	Koren = NULL;
}

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
	if (source->typ == 3){
		if (source->value.s != NULL){			
		int len = strlen(source->value.s);
			char* tmp = malloc(len+1);
			strcpy(tmp, source->value.s);
			ukazatel->value.s = tmp;
		}
	}
	else ukazatel->value = source->value;
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

void znic_parametry(param *parametr){
	if (parametr->next != NULL) znic_parametry(parametr->next);
	free(parametr->name);
	free(parametr);
}



/*
* Funkce vraci delku retezce
* @par1 vstupni retezec
*/
int length(char *s){
	return strlen(s);
}

/*
* Funkce pro hledani indexu podretezce
* @par1 vstupni string
* @par2 hledany string
* @return 0 v pripade neuspechu, index v pripade uspechu
*/
int find(char* string, char* pattern){ // boyer moore
    char* inits = string;
    char* initp = pattern;
	if (length(pattern) == 0){
		return 0;
	}

    int spatt = length(pattern);
    while(*pattern != '\0') pattern++;
    int* jmp_table=(int*) calloc(128, sizeof(int));
    int count=0;

    while(pattern != initp) {
        pattern--;
        jmp_table[*pattern]=count;
        count++;
    }

    char* stmp = 0;
    char* iter = 0;
    int shift = 0;
    int bad_count = 0;
    int bcount= 0;
    while(*string != '\0')
    {
        bcount = 0;
        bad_count = spatt;
        stmp = string+ (spatt-1);
        iter = pattern + (spatt-1);
        while(*stmp == *iter) {
            bad_count--;
            bcount++;
            stmp--;
            iter--;
            if(bcount == spatt)
                return string-inits+1; // vraci nalez od jednicky
        }

        if(jmp_table[*stmp] == 0) {
            // znak nebyl nalezen
            shift = bad_count;
        } else {
            shift=jmp_table[*stmp];
            if ((shift - bcount) < 1){
            	shift = 1;
			}else{
				shift = shift - bcount;
			}

        }
        string += shift;
    }
    //nenalezen
    return -1;
}
/*
* Funkce vraci podretezc zadaneho retezce
* @par1 vstupni retezec
* @par2 pocatecni index retezce
* @par3 koncovy index retezce
* @return - vraci podretezec
*/
char *substr(char *s, int i, int n){ // funkcni
	char *retez_ret;
	retez_ret = (char *)malloc(sizeof(char)*(n+1));
	int poc = 0;
	int z = i-1;
	for (int k = 0 ; k < n; k++){
		retez_ret[poc] = s[z];
		poc++;
		z++;
	}
	retez_ret[poc] = '\0';
	return retez_ret;
	free(retez_ret);
}

/*
* Funkce na konkatenaci retezcu
* @par1 vstupni retezec1
* @par2 vstupni retezc2
* @return konkatenovany retezec
*/
char *concat(char *s1, char *s2){
	int delka = length(s1) + length(s2);
	char *vysledek;
	int i = 0;
	vysledek = (char *)malloc(sizeof(char)*(delka+1));
	for (int x = 0; x < length(s1); x++)
	{
		vysledek[i] = s1[x];
		i++;
	}
	for (int x = 0; x < length(s2); x++)
	{
		vysledek[i] = s2[x];
		i++;
	}
	vysledek[i] = '\0';
	return vysledek;
}
/*
* Funkce vrací seřazenou posloupnost od nejmnsi po nejvetsi
* @par1 vstupni retezec
* @ return serazeny retezec
*/
char *sort(char *string){ //shell-sort - plne funkcni
	int delka = length(string);
	int i = 0, j = 0, k = 0, mid = 0;
	for(k = delka/2; k > 0; k /= 2)
	{
		for (j =  k; j < delka; j++)
		{
			for (i = j - k; i >= 0; i -= k)
			{
				if ((int*)string[i + k] >= (int*)string[i]){
					break;
				}
				else{
					mid = string[i];
					string[i] = string[i + k];
					string[i + k] = mid;
				}
			}
		}
	}
	return string;
}