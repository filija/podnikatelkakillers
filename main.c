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
   if (result) {
      fclose(f);
      znic_tabulku(global_table);
      listFree(&inst_list);
      return -result;
   }
   result = check_defined(global_table);
   if (result) {
      fclose(f);
      znic_tabulku(global_table);
      listFree(&inst_list);
      return -result;
   }
   result = interpret(inst_list,global_table);
      if (result) {
      fclose(f);
      znic_tabulku(global_table);
      listFree(&inst_list);
      return -result;
   }
   fclose(f);
   znic_tabulku(global_table);
   listFree(&inst_list);
   return -result;
}