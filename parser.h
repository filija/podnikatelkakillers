typedef struct listPrecedence {
    struct listPrecedence *next;
    int s_token;
    char *promenna;
} *PrcPtr;

void generateVariable(char *var);
int pushPrc(int s_token, char* promenna, PrcPtr list);
int popPrc(int* s_token, char* promenna, PrcPtr list);
void emptyPrc(PrcPtr list);
int topPrc(PrcPtr list);
int poptPrc(PrcPtr list);
int syntax_precedencka();
int prec_prevod(int* token, char* attrc);
//int parse(struct symbol_table* table_hl, tList *list, ptrStack stack_hl); //
int parse();
int program();
int dfunkce();
int funkce();
int deklarace();
int typ();
int parametry();
int dparametr();
int parametr();
int slozeny();
int prikazy();
int definice();
int prikaz();
int prirazeni();
int nacteni();
int dnacteni();
int vypis();
int dvypis();
int promenna();
int prirad();
int termy();
int dterm();
int term();
