#include "interpret.h"

int interpret(tListOfInstr inst_list, uk_uzel global_table){
	char in_buffer[BUFFER_LEN];
	int in_status = 0;
	int in_position = 0;
	tStackPtr lokalni_tabulky = NULL;
	ZIPtr instr_stack = NULL;
	FrontaPtr fronta = NULL;
	tSymbolPtr start = najdi_v_tabulce(global_table, "main");
	if (start == NULL) return SEM_ERR;
	if (start->parametry != NULL) return SEM_ERR;
	/*listFirst(&inst_list);
	while (inst_list.active != NULL){
		printf("instr number %i %s %s %s\n", inst_list.active->instType, (char*)inst_list.active->addr1, (char*)inst_list.active->addr2, (char*)inst_list.active->addr3);
		listNext(&inst_list);
	}*/
	listJump(&inst_list, start->label);
	push_tstack(&lokalni_tabulky, start->tabulka, 1);
	push_tstack(&lokalni_tabulky, NULL, 0);
	while(lokalni_tabulky != NULL){
	if (in_status) {
		if (in_status == 1) in_position = 0;
		in_status--;
	}
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
						tmp3->value.i = tmp1->value.i / tmp2->value.i;
					}
					else{
						tmp3->value.d = (double)tmp1->value.i / tmp2->value.d;
					}
				}
				else{
					if (tmp2->typ == INT_V){
						tmp3->value.d = tmp1->value.d / (double)tmp2->value.i;
					}
					else{
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
				continue;
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
				pop_tstack(&lokalni_tabulky);	//free na tuto tabulku
				pop_tstack(&lokalni_tabulky);
				break;
			case I_PRIRAZENI:
				tmp1 = find_tstack(lokalni_tabulky, aktualni->addr1);
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
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
					tmp3->value.s = tmp1->value.s;
				}
				break;
			case I_CTENI://#rup curak, zvlastni stavovy automat, ktery si bude volat cteni pokud je vstup prazdny... to co je napsane je k nicemu
				tmp3 = out_find_tstack(lokalni_tabulky, aktualni->addr3);
				if (in_status == 0){
					fgets(in_buffer, BUFFER_LEN, stdin);
					in_status++;
					printf("%s", in_buffer);
				}
				switch (tmp3->typ){
					case INT_V:;
							int vystup = -1;
							int result = cin_int(&vystup, in_buffer, &in_position);
							printf("%i res: %i\n", vystup, result);
						break;
					case DOUBLE_V:
						
						break;
					case STRING_V:
						
						break;					
				}
				in_status++;
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
			case I_RETURN:	//vyresit jeste pop na seznam!
				tmp3 = find_tstack(lokalni_tabulky, aktualni->addr3);
				copy_item(&tmp2, tmp3);
				pushFronta(&fronta, tmp2);
				pop_tstack(&lokalni_tabulky);	//free na tuto tabulku
				while (lokalni_tabulky->zarazka != 1){
					pop_tstack(&lokalni_tabulky);
					pop_tstack(&lokalni_tabulky);
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
					tmp3->value.s = tmp2->value.s;
				}
				break;
			case I_CALL:
				tmp3 = najdi_v_tabulce(global_table, aktualni->addr3);
				push_tstack(&lokalni_tabulky, tmp3->tabulka, 1);
				push_tstack(&lokalni_tabulky, NULL, 0);
				pushZI(&instr_stack, aktualni);
				listJump(&inst_list, tmp3->label);
				break;
			default:	//label!
				break;
		}
		listNext(&inst_list);
	 }
	 return IS_OK;
}

int pushFronta(FrontaPtr* fronta, tSymbolPtr prvek){
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

tSymbolPtr popFronta(FrontaPtr* fronta){
	if (*fronta == NULL) return NULL;
	tSymbolPtr result = (*fronta)->symbol;
	FrontaPtr tmpfronta = *fronta;
	*fronta = (*fronta)->next;
	free(tmpfronta);
	return result;
}

int pushZI(ZIPtr* Instr_zas, tInstr* instrukce){
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

tInstr* popZI(ZIPtr *Instr_zas){
	if (*Instr_zas == NULL) return NULL;
	tInstr* result = (*Instr_zas)->instrukce;
	ZIPtr tmpprvek = *Instr_zas;
	*Instr_zas = (*Instr_zas)->next;
	free(tmpprvek);
	return result;
}

int cin_int(int *vystup, char *vstup, int *pozice){
	while (isspace(vstup[*pozice])) *pozice = *pozice + 1;
	int poc_poz = *pozice;
	while (isdigit(vstup[*pozice])) *pozice = *pozice + 1;
	if (poc_poz == *pozice) return CIN_ERR;
	*vystup = atoi(vstup+poc_poz);
	return IS_OK;
}