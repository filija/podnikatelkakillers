#include <string.h>
#include "tabulka_symbolu.h"
#include "instruction_list.h"
#include "define.h"

typedef struct FrontaInter
{
    tSymbolPtr symbol;
    struct FrontaInter *next;
}*FrontaPtr;

int pushFronta(FrontaPtr* fronta, tSymbolPtr prvek);
tSymbolPtr popFronta(FrontaPtr* fronta);
int interpret(tListOfInstr inst_list, uk_uzel global_table);