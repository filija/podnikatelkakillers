#include <stdio.h>
#include "str.h"
#include "parser.h"
#include "scanner.h"
#include "define.h"

int main(int argc, char** argv)
{
    FILE *f;
   if (argc == 1)
   {
      printf("Neni zadan vstupni soubor\n");
      return INTERNAL_ERR;
   }
   if ((f = fopen(argv[1], "r")) == NULL)
   {
      printf("Soubor se nepodarilo otevrit\n");
      return INTERNAL_ERR;
   }   
   setSourceFile(f);
   int result;
   result = parse();
   printf("result: %i\n", result);
   fclose(f);
   if (result) return -result;
   return 0;
}
