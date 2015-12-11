#include "interpret.h"
tStackPtr lokalni_tabulky = NULL;

int interpret(tListOfInstr inst_list, uk_uzel global_table){
	tSymbolPtr start = najdi_v_tabulce(global_table, "main");
	if (start == NULL) return SEM_ERR;
	if (start->parametry != NULL) return SEM_ERR;
	listJump(&inst_list, start->label);
	while (inst_list.active != NULL){
      printf("instr number %i %s %s %s\n", inst_list.active->instType, (char*)inst_list.active->addr1, (char*)inst_list.active->addr2, (char*)inst_list.active->addr3);
      listNext(&inst_list);
    }
    listJump(&inst_list, start->label);
    push_tstack(&lokalni_tabulky, start->tabulka, 1);
    push_tstack(&lokalni_tabulky, NULL, 0);
    while(lokalni_tabulky != NULL){
    tInstr* aktualni = inst_list.active;
    tSymbolPtr tmp1;
    tSymbolPtr tmp2;
    tSymbolPtr tmp3;
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
   					//
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
   			case I_CTENI://vyresit problemy s formatama atd... pouzit lex na cisla, stringy poresit sam
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
   				pop_tstack(&lokalni_tabulky);	//free na tuto tabulku
   				pop_tstack(&lokalni_tabulky);
   				break;
   			case I_FUN_ERR:
   				return NONINC_ERR;
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
		*fronta =  malloc(sizeof(struct FrontaInter));
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