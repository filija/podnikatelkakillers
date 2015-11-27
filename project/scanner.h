typedef struct{
  char* str;        // misto pro dany retezec ukonceny znakem '\0'
  int length;       // skutecna delka retezce
  int allocSize;    // velikost alokovane pameti
} string;

void setSourceFile(FILE *f);
void lErr();
int getNextToken(string *attr);