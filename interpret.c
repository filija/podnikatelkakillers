/*
*	IFJ Projekt
*		scanner pro jazyk IFJ15
*		autor: Jakub Korgo, Martin Kruták
*		login: xkorgo01, xkruta03
*/


#include "interpret.h"

int interpret(tListOfInstr inst_list, uk_uzel global_table){	//smycka vykonavajici instrukce
	char in_buffer[BUFFER_LEN] = "\n";	//promenne pro udrzovani infromaci o vstupu od uzivatele
	int in_position = 0;
	tStackPtr lokalni_tabulky = NULL;	//zasobnik tabulek, ktery resi zanorovani a rekurzi
	ZIPtr instr_stack = NULL;	//zasobnik navratovych skoku
	FrontaPtr fronta = NULL;	//Fronta na predavani dat mezi funkcemi
	tSymbolPtr start = najdi_v_tabulce(global_table, "main");	//Vyhledani funkce main + kontrola jestli jsou spravne nezadane parametry
	if (start == NULL) return SEM_ERR;
	if (start->parametry != NULL) return SEM_ERR;
	listJump(&inst_list, start->label);	//nastaveni pocatecni instrukce
	push_tstack(&lokalni_tabulky, start->tabulka, 1);	//nacteni vstupnich dat mainu
	push_tstack(&lokalni_tabulky, NULL, 0);	//nastaveni pracovni tabulky main
	while(lokalni_tabulky != NULL){
	tInstr* aktualni = inst_list.active;
	tSymbolPtr tmp1 = NULL;
	tSymbolPtr tmp2 = NULL;
	tSymbolPtr tmp3 = NULL;
		switch (aktualni->instType){
			case I_SCITANI:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = tmp2->value.i + tmp1->value.i;
					}
					else{
						tmp3->value.d = tmp2->value.d + tmp1->value.i;
					}
				}
				else{
					if (tmp2->typ == INT_V){
						tmp3->value.d = tmp2->value.i + tmp1->value.d;
					}
					else{
						tmp3->value.d = tmp2->value.d + tmp1->value.d;
					}
				}
				break;
			case I_ODCITANI:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = tmp1->value.i - tmp2->value.i;
					}
					else{
						tmp3->value.d = (double)tmp1->value.i - tmp2->value.d;
					}
				}
				else{
					if (tmp2->typ == INT_V){
						tmp3->value.d = tmp1->value.d - (double)tmp2->value.i;
					}
					else{
						tmp3->value.d = tmp1->value.d - tmp2->value.d;
					}
				}
				break;
			case I_NASOBENI:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = tmp2->value.i * tmp1->value.i;
					}
					else{
						tmp3->value.d = tmp2->value.d * (double)tmp1->value.i;
					}
				}
				else{
					if (tmp2->typ == INT_V){
						tmp3->value.d = (double)tmp2->value.i * tmp1->value.d;
					}
					else{
						tmp3->value.d = tmp2->value.d * tmp1->value.d;
					}
				}
				break;
			case I_DELENI:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						if (tmp2->value.i == 0) return DIVZERO_ERR;
						tmp3->value.i = tmp1->value.i / tmp2->value.i;
					}
					else{
						if (tmp2->value.d == 0.0) return DIVZERO_ERR;
						tmp3->value.d = (double)tmp1->value.i / tmp2->value.d;
					}
				}
				else{
					if (tmp2->typ == INT_V){
						if (tmp2->value.i == 0) return DIVZERO_ERR;
						tmp3->value.d = tmp1->value.d / (double)tmp2->value.i;
					}
					else{
						if (tmp2->value.d == 0.0) return DIVZERO_ERR;
						tmp3->value.d = tmp1->value.d / tmp2->value.d;
					}
				}
				break;
		case I_MENSI:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.i < tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = ((double)tmp1->value.i < tmp2->value.d) ? 1 : 0;
					}
				}
				else if (tmp1->typ == DOUBLE_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.d < (double)tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = (tmp1->value.d < tmp2->value.d)  ? 1 : 0;
					}
				}
				else{
					tmp3->value.i = (strcmp(tmp1->value.s, tmp2->value.s) < 0) ? 1 : 0;
				}
				break;
			case I_MENSI_NEBO:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.i <= tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = ((double)tmp1->value.i <= tmp2->value.d) ? 1 : 0;
					}
				}
				else if (tmp1->typ == DOUBLE_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.d <= (double)tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = (tmp1->value.d <= tmp2->value.d)  ? 1 : 0;
					}
				}
				else{
					tmp3->value.i = (strcmp(tmp1->value.s, tmp2->value.s) <= 0) ? 1 : 0;
				}
				break;
			case I_VETSI:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.i > tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = ((double)tmp1->value.i > tmp2->value.d) ? 1 : 0;
					}
				}
				else if (tmp1->typ == DOUBLE_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.d > (double)tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = (tmp1->value.d > tmp2->value.d)  ? 1 : 0;
					}
				}
				else{
					tmp3->value.i = (strcmp(tmp1->value.s, tmp2->value.s) > 0) ? 1 : 0;
				}
				break;
			case I_VETSI_NEBO:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.i >= tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = ((double)tmp1->value.i >= tmp2->value.d) ? 1 : 0;
					}
				}
				else if (tmp1->typ == DOUBLE_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.d >= (double)tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = (tmp1->value.d >= tmp2->value.d)  ? 1 : 0;
					}
				}
				else{
					tmp3->value.i = (strcmp(tmp1->value.s, tmp2->value.s) >= 0) ? 1 : 0;
				}
				break;
			case I_ROVNO:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.i == tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = ((double)tmp1->value.i == tmp2->value.d) ? 1 : 0;
					}
				}
				else if (tmp1->typ == DOUBLE_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.d == (double)tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = (tmp1->value.d == tmp2->value.d)  ? 1 : 0;
					}
				}
				else{
					tmp3->value.i = (strcmp(tmp1->value.s, tmp2->value.s) == 0) ? 1 : 0;
				}
				break;
			case I_NEROVNO:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp2 = find_tstack(lokalni_tabulky, aktualni->addr2);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.i != tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = ((double)tmp1->value.i != tmp2->value.d) ? 1 : 0;
					}
				}
				else if (tmp1->typ == DOUBLE_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = (tmp1->value.d != (double)tmp2->value.i) ? 1 : 0;
					}
					else{
						tmp3->value.i = (tmp1->value.d != tmp2->value.d)  ? 1 : 0;
					}
				}
				else{
					tmp3->value.i = (strcmp(tmp1->value.s, tmp2->value.s) != 0) ? 1 : 0;
				}
				break;
			case I_SKOK:
				tmp3 = find_tstack(lokalni_tabulky, aktualni->addr3);
				listJump(&inst_list, tmp3->label);
				continue;	//continue z duvodu aby jsme se vyhli preskoceni instrukce na kterou se skace, hlavne kvuli instrukci Label_Load
			case I_PODM_SKOK:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp3 = find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp1->typ == INT_V){
					if (tmp1->value.i == 0){
						listJump(&inst_list, tmp3->label);
						continue;
					}
				}
				else {
					if ((int)tmp1->value.d == 0){
						listJump(&inst_list, tmp3->label);
						continue;
					}
				}
				break;
			case I_LABEL_LOAD:
				tmp3 = find_tstack(lokalni_tabulky, aktualni->addr3);
				push_tstack(&lokalni_tabulky, tmp3->tabulka, 0);
				push_tstack(&lokalni_tabulky, NULL, 0);
				break;
			case I_UNLOAD:
				znic_pracovni_tabulku(lokalni_tabulky->tabulka);
				pop_tstack(&lokalni_tabulky);	//free na tuto tabulku
				pop_tstack(&lokalni_tabulky);
				break;
			case I_PRIRAZENI:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp3 = out_find_tstack(lokalni_tabulky, (char*)aktualni->addr3);
				if (tmp3->typ == INT_V){
					if (tmp1->typ == INT_V){
						tmp3->value.i = tmp1->value.i;
					}
					else{
						tmp3->value.i = (int)tmp1->value.d;
					}
				}
				else if (tmp3->typ == DOUBLE_V){
					if (tmp1->typ == INT_V){
						tmp3->value.d = (double)tmp1->value.i;
					}
					else{
						tmp3->value.d = tmp1->value.d;
					}
				}
				else {
					if (tmp3->value.s != NULL) free(tmp3->value.s);
					char* tmpchar = malloc(strlen(tmp1->value.s)+1);
					strcpy(tmpchar, tmp1->value.s);
					tmp3->value.s = tmpchar;
				}
				break;
			case I_CTENI:
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				int result;
				switch (tmp3->typ){
					case INT_V:;
							int vystupi;
							result = cin_int(&vystupi, in_buffer, &in_position);
							if (result != IS_OK) return result;
							tmp3->value.i = vystupi;
						break;
					case DOUBLE_V:;
							double vystupd;
							result = cin_float(&vystupd, in_buffer, &in_position);
							if (result != IS_OK) return result;
							tmp3->value.d = vystupd;
						break;
					case STRING_V:;
							char* vystupc;
							result = cin_string(&vystupc, in_buffer, &in_position);
							if (result != IS_OK) return result;
							if (tmp3->value.s == NULL)tmp3->value.s = vystupc;
							else {
								free(tmp3->value.s);
								tmp3->value.s = vystupc;
							}
						break;					
				}
				break;
			case I_ZAPIS:
				tmp3 = find_tstack(lokalni_tabulky, aktualni->addr3);
				switch (tmp3->typ){
					case INT_V:
						printf("%i", tmp3->value.i);
						break;
					case DOUBLE_V:
						printf("%g", tmp3->value.d);
						break;
					case STRING_V:
						printf("%s", tmp3->value.s);
						break;
				}
				break;
			case I_RETURN:
				tmp3 = find_tstack(lokalni_tabulky, aktualni->addr3);
				copy_item(&tmp2, tmp3);
				pushFronta(&fronta, tmp2);
				znic_pracovni_tabulku(lokalni_tabulky->tabulka);
				pop_tstack(&lokalni_tabulky);	//free na tuto tabulku
				while (lokalni_tabulky->zarazka != 1){
					pop_tstack(&lokalni_tabulky);
					znic_pracovni_tabulku(lokalni_tabulky->tabulka);
					pop_tstack(&lokalni_tabulky);	//i na tuto free
				}
				pop_tstack(&lokalni_tabulky);
				aktualni = popZI(&instr_stack);
				listJump(&inst_list, aktualni);
				break;
			case I_FUN_ERR:
				return NONINC_ERR;
				break;
			case I_PUSH:
				tmp3 = find_tstack(lokalni_tabulky, aktualni->addr3);
				copy_item(&tmp2, tmp3);
				pushFronta(&fronta, tmp2);
				break;
			case I_POP:
				tmp2 = popFronta(&fronta);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (tmp3->typ == INT_V){
					if (tmp2->typ == INT_V){
						tmp3->value.i = tmp2->value.i;
					}
					else{
						tmp3->value.i = (int)tmp2->value.d;
					}
				}
				else if (tmp3->typ == DOUBLE_V){
					if (tmp2->typ == INT_V){
						tmp3->value.d = (double)tmp2->value.i;
					}
					else{
						tmp3->value.d = tmp2->value.d;
					}
				}
				else {
					if (tmp3->value.s != NULL) free(tmp3->value.s);
					tmp3->value.s = tmp2->value.s;
				}
				break;
				free(tmp2);
			case I_CALL:
				tmp3 = najdi_v_tabulce(global_table, aktualni->addr3);
				push_tstack(&lokalni_tabulky, tmp3->tabulka, 1);
				push_tstack(&lokalni_tabulky, NULL, 0);
				pushZI(&instr_stack, aktualni);
				listJump(&inst_list, tmp3->label);
				break;
			default:	//label
				break;
		}
		listNext(&inst_list);
	 }
	 return IS_OK;
}

int pushFronta(FrontaPtr* fronta, tSymbolPtr prvek){	//resi vlozeni na frontu pomoci ktere si predavaji data funkce mezi sebou
	if (*fronta == NULL){
		*fronta = malloc(sizeof(struct FrontaInter));
		if (*fronta != NULL){
			(*fronta)->symbol = prvek;
			(*fronta)->next = NULL;
			return IS_OK;
		}
		return INTERNAL_ERR;
	}
	FrontaPtr tmpprvek = malloc(sizeof(struct FrontaInter));
	FrontaPtr tmpfronta = *fronta;
	if (tmpprvek!=NULL){
		tmpprvek->symbol = prvek;
		tmpprvek->next = NULL;
		while(tmpfronta->next != NULL){
			tmpfronta = tmpfronta->next;
		}
		tmpfronta->next = tmpprvek;
		return IS_OK;
	}
	return INTERNAL_ERR;
}

tSymbolPtr popFronta(FrontaPtr* fronta){	//resi vyjmuti z fronty pomoci ktere si predavaji data funkce mezi sebou
	if (*fronta == NULL) return NULL;
	tSymbolPtr result = (*fronta)->symbol;
	FrontaPtr tmpfronta = *fronta;
	*fronta = (*fronta)->next;
	free(tmpfronta);
	return result;
}

int pushZI(ZIPtr* Instr_zas, tInstr* instrukce){	//vlozeni na zaobnik skokovych instrukci
	if (*Instr_zas == NULL){
		*Instr_zas = malloc(sizeof(struct ZasobnikInstrukci));
		if (*Instr_zas != NULL){
			(*Instr_zas)->instrukce = instrukce;
			(*Instr_zas)->next = NULL;
			return IS_OK;
		}
		return INTERNAL_ERR;
	}
	ZIPtr tmpprvek = malloc(sizeof(struct ZasobnikInstrukci));
	if (tmpprvek != NULL){
		tmpprvek -> instrukce = instrukce;
		tmpprvek -> next = *Instr_zas;
		*Instr_zas = tmpprvek;
	}
	return INTERNAL_ERR;
}

tInstr* popZI(ZIPtr *Instr_zas){	//vyjmuti ze zasobniku skokovych instrukci
	if (*Instr_zas == NULL) return NULL;
	tInstr* result = (*Instr_zas)->instrukce;
	ZIPtr tmpprvek = *Instr_zas;
	*Instr_zas = (*Instr_zas)->next;
	free(tmpprvek);
	return result;
}

int cin_int(int *vystup, char *vstup, int *pozice){	//nacteni int ze vstupu a v pripade zadnych dat, si znovu vola nacteni
	int stav = 0;
	int poc_pozice = 0;
	while (1){
		switch (stav){
			case 0:
				if (vstup[*pozice] == '\n') stav = 1;
				else if (isspace(vstup[*pozice])) *pozice = *pozice + 1;
				else if (isdigit(vstup[*pozice])){
					poc_pozice = *pozice;
					*pozice = *pozice + 1;
					stav = 2;
				}
				else return CIN_ERR;
				break;
			case 1:
				fgets(vstup, BUFFER_LEN, stdin);
				*pozice = 0;
				stav = 0;
				break;
			case 2:
				if (isdigit(vstup[*pozice])) *pozice = *pozice + 1;
				else {
					*vystup = atoi(vstup+poc_pozice);
					return IS_OK;
				}
		}
	}
}

int cin_float(double *vystup, char *vstup, int *pozice){ //nacteni float ze vstupu a v pripade zadnych dat, si znovu vola nacteni
	int stav = 0;
	int poc_pozice = 0;
	while (1){
		switch (stav){
			case 0:
				if (vstup[*pozice] == '\n') stav = 1;
				else if (isspace(vstup[*pozice])) *pozice = *pozice + 1;
				else if (isdigit(vstup[*pozice])){
					poc_pozice = *pozice;
					*pozice = *pozice + 1;
					stav = 2;
				}
				else return CIN_ERR;
				break;
			case 1:
				fgets(vstup, BUFFER_LEN, stdin);
				*pozice = 0;
				stav = 0;
				break;
			case 2:
				if (isdigit(vstup[*pozice])) *pozice = *pozice + 1;
				else if (vstup[*pozice] == '.'){
					*pozice = *pozice + 1;
					stav = 3;
				}
				else if (vstup[*pozice] == 'e' || vstup[*pozice] == 'E'){
					*pozice = *pozice + 1;
					stav = 5;
				}
				else return CIN_ERR;
				break;
			case 3:
				if (isdigit(vstup[*pozice])){
					*pozice = *pozice + 1;
					stav = 4;
				}
				else return CIN_ERR;
				break;
			case 4:
				if (isdigit(vstup[*pozice])) *pozice = *pozice + 1;
				else if (vstup[*pozice] == 'e' || vstup[*pozice] == 'E'){
					*pozice = *pozice + 1;
					stav = 5;
				}
				else {
					*vystup = atof(vstup+poc_pozice);
					return IS_OK;
				}
				break;
			case 5:
				if (isdigit(vstup[*pozice])){
					*pozice = *pozice + 1;
					stav = 7;
				}
				else if (vstup[*pozice] == '+' || vstup[*pozice] == '-'){
					*pozice = *pozice + 1;
					stav = 6;
				}
				else return CIN_ERR;
				break;
			case 6:
				if (isdigit(vstup[*pozice])){
					*pozice = *pozice + 1;
					stav = 7;
				}
				else return CIN_ERR;
				break;
			case 7:
				if (isdigit(vstup[*pozice])) *pozice = *pozice + 1;
				else {
					*vystup = atof(vstup+poc_pozice);
					return IS_OK;
				}
				break;
		}
	}
}

int cin_string(char **vystup, char *vstup, int *pozice){ //nacteni float ze vstupu a v pripade zadnych novych dat, vraci prazdny retezec
	int stav = 0;
	int poc_pozice = 0;
	int flag = 0;
	while (1){
		switch (stav){
			case 0:
				if (vstup[*pozice] == '\n') stav = 1;
				else if (isspace(vstup[*pozice])) *pozice = *pozice + 1;
				else {
					poc_pozice = *pozice;
					*pozice = *pozice + 1;
					stav = 2;
				}
				break;
			case 1:
				if (flag){
					*vystup = malloc(sizeof(char));
					if (*vystup == NULL) return INTERNAL_ERR;
					*vystup[0] = 0;
					return IS_OK;
				}
				else{
					fgets(vstup, BUFFER_LEN, stdin);
					*pozice = 0;
					stav = 0;
					flag = 1;
				}
			break;
			case 2:
				if (isspace(vstup[*pozice])){
					*vystup = malloc(*pozice - poc_pozice + 1);
					if (*vystup == NULL) return INTERNAL_ERR;
					strncpy (*vystup, vstup+poc_pozice, *pozice - poc_pozice);
					(*vystup)[*pozice - poc_pozice] = '\0';
					return IS_OK;
				}
				else {
					*pozice = *pozice + 1;
				}
			break;
		}
	}
}