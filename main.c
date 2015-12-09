#include <stdio.h>
#include "str.h"
#include "parser.h"
#include "scanner.h"
#include "tabulka_symbolu.h"
#include "instruction_list.h"
#include "define.h"

int main(int argc, char** argv)
{
   FILE *f;
   if (argc == 1)
   {
      printf("Neni zadan vstupni soubor\n");
      return - INTERNAL_ERR;
   }
   if ((f = fopen(argv[1], "r")) == NULL)
   {
      printf("Soubor se nepodarilo otevrit\n");
      return - INTERNAL_ERR;
   }   
   setSourceFile(f);
   uk_uzel global_table=NULL; 
   tListOfInstr inst_list;
   listInit(&inst_list);
   int result;
   result = parse(&global_table, &inst_list);
   /*listFirst(&inst_list);
   for (int i = 0; i<3; i++){
      printf("instr number %i\n", inst_list.active->instType);
      listNext(&inst_list);
   }
   printf("Result: %i\n", result);*/
   fclose(f);
   if (result) return -result;
   return 0;
}
