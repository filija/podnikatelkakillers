#include "str.h"

typedef struct listPrecedence {
    struct listPrecedence *next;
    int s_token;
    char *promenna;
} *PrcPtr;

void generateVariable(string *var);
int pushPrc(int s_token, char* promenna, PrcPtr list);
int popPrc(int* s_token, char* promenna, PrcPtr list);
void emptyPrc(PrcPtr list);
int topPrc(PrcPtr list);
int poptPrc(PrcPtr list);
int syntax_precedencka();
int prec_prevod(int* token, char* attrc);