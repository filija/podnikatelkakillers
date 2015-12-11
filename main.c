#include <stdio.h>
#include "str.h"
#include "parser.h"
#include "scanner.h"
#include "tabulka_symbolu.h"
#include "instruction_list.h"
#include "interpret.h"
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
   printf("Result: %i\n", result);
   if (result) return -result;
   result = check_defined(global_table);
   printf("Result: %i\n", result);
   if (result) return -result;
   listFirst(&inst_list);
   /*while (inst_list.active != NULL){
      printf("instr number %i %s %s %s\n", inst_list.active->instType, (char*)inst_list.active->addr1, (char*)inst_list.active->addr2, (char*)inst_list.active->addr3);
      listNext(&inst_list);
   }*/
   result = interpret(inst_list,global_table);
   printf("Result: %i\n", result);
   if (result) return -result;
   fclose(f);
   return 0;
}

//PUSH POP MUSI BYT SEZNAM, NE ZASOBIK!!!!