/*
* Implementace Shell-Sortu a Boyer-Mooreuv algortimus vyhledavani
*/

#include "ial.h"
#include <stdio.h>

/*
* Funkce vraci delku retezce
* @par1 vstupni retezec
*/
int length(char *s){
	return strlen(s);
}

/*
* Funkce pro hledani indexu podretezce
* @par1 vstupni string
* @par2 hledany string
* @return 0 v pripade neuspechu, index v pripade uspechu
*/
int find(char* string, char* pattern){ // boyer moore
    char* inits = string;
    char* initp = pattern;
	if (length(pattern) == 0){
		return 0;
	}

    int spatt = length(pattern);
    while(*pattern != '\0') pattern++;
    int* jmp_table=(int*) calloc(128, sizeof(int));
    int count=0;

    while(pattern != initp) {
        pattern--;
        jmp_table[*pattern]=count;
        count++;
    }

    char* stmp = 0;
    char* iter = 0;
    int shift = 0;
    int bad_count = 0;
    int bcount= 0;
    while(*string != '\0')
    {
        bcount = 0;
        bad_count = spatt;
        stmp = string+ (spatt-1);
        iter = pattern + (spatt-1);
        while(*stmp == *iter) {
            bad_count--;
            bcount++;
            stmp--;
            iter--;
            if(bcount == spatt)
                return string-inits+1; // vraci nalez od jednicky
        }

        if(jmp_table[*stmp] == 0) {
            // znak nebyl nalezen
            shift = bad_count;
        } else {
            shift=jmp_table[*stmp];
            if ((shift - bcount) < 1){
            	shift = 1;
			}else{
				shift = shift - bcount;
			}

        }
        string += shift;
    }
    //nenalezen
    return -1;
}
/*
* Funkce vraci podretezc zadaneho retezce
* @par1 vstupni retezec
* @par2 pocatecni index retezce
* @par3 koncovy index retezce
* @return - vraci podretezec
*/
char *substr(char *s, int i, int n){ // funkcni
	char *retez_ret;
	retez_ret = (char *)malloc(sizeof(char)*(n+1));
	int poc = 0;
	int z = i-1;
	for (int k = 0 ; k < n; k++){
		retez_ret[poc] = s[z];
		poc++;
		z++;
	}
	retez_ret[poc] = '\0';
	return retez_ret;
	free(retez_ret);
}

/*
* Funkce na konkatenaci retezcu
* @par1 vstupni retezec1
* @par2 vstupni retezc2
* @return konkatenovany retezec
*/
char *concat(char *s1, char *s2){
	int delka = length(s1) + length(s2);
	char *vysledek;
	int i = 0;
	vysledek = (char *)malloc(sizeof(char)*(delka+1));
	for (int x = 0; x < length(s1); x++)
	{
		vysledek[i] = s1[x];
		i++;
	}
	for (int x = 0; x < length(s2); x++)
	{
		vysledek[i] = s2[x];
		i++;
	}
	vysledek[i] = '\0';
	return vysledek;
}
/*
* Funkce vrací seřazenou posloupnost od nejmnsi po nejvetsi
* @par1 vstupni retezec
* @ return serazeny retezec
*/
char *sort(char *string){ //shell-sort - plne funkcni
	int delka = length(string);
	int i = 0, j = 0, k = 0, mid = 0;
	for(k = delka/2; k > 0; k /= 2)
	{
		for (j =  k; j < delka; j++)
		{
			for (i = j - k; i >= 0; i -= k)
			{
				if ((int*)string[i + k] >= (int*)string[i]){
					break;
				}
				else{
					mid = string[i];
					string[i] = string[i + k];
					string[i + k] = mid;
				}
			}
		}
	}
	return string;
}

void main(){
	char *str = "Nekde je podretezec v tomto miste?";
	char *str2 = "kaliste";
	char *str3 = "podretezec";
	char *strr = "";
	int pozice = 0;
	printf("**********************************\n");
	printf("Test implementace funkci \n");
	printf("**********************************\n\n");
	pozice = find(str, str2);
	printf(">> Test boyer-moore (negativni) \n");
	printf("	Vychozi retezec: %s \n", str);
	printf("	Hledany retezec: %s \n", str2);
	printf("	Nalezena pozice: %d \n", pozice);
	pozice = find(str, str3);
	printf(">> Test boyer-moore (pozitivni) \n");
	printf("	Vychozi retezec: %s \n", str);
	printf("	Hledany retezec: %s \n", str3);
	printf("	Nalezena pozice: %d \n", pozice);
	pozice = find(str, strr);
	printf(">> Test boyer-moore (prazdny retezec) \n");
	printf("	Vychozi retezec: %s \n", str);
	printf("	Hledany retezec: %s \n", strr);
	printf("	Nalezena pozice: %d \n\n", pozice);
	printf("------------------------------------- \n\n");
	printf(">> Test funkce length \n");
	char *str4 = "x\nz";
	printf("	Vychozi retezec: %s - retezec x\\nz \n", str4);
	pozice = length(str4);
	printf("	Delka: %d \n\n", pozice);
	printf("------------------------------------- \n\n");
	printf(">> Test funkce substr \n");
	char *str5 = "Lorem impsum dolor sit amed";
	int delka = 5;
	int pocatek = 4;
	printf("	Vychozi retezec: %s \n", str5);
	printf("	Pozadovana delka podretezce: %d \n", delka);
	printf("	Pozadovana pocatecni pozice: %d \n", pocatek);
	printf("	Vysledny podretezec: %s \n\n", substr(str5, pocatek, delka));
	printf("------------------------------------- \n\n");
	printf(">> Test funkce concat \n");
	char *str6 = "To je dnes ale ";
	char *str7 = "krasne!";
	printf("	Retezec 1: %s \n", str6);
	printf("	Retezec 2: %s \n", str7);
	printf("	Konkatenace: %s \n\n", concat(str6, str7));
	printf("------------------------------------- \n\n");
	printf(">> Test funkce sort \n");
	char str8[] = "avcgjmq&'$%4@b^DCdkcorpwc5768/**=z+;;<>-_:?!)/)({}[]AHRTZ";
	printf("	Vstupni retezec: %s \n", str8);
	printf("	Serazeny retezec: %s \n\n", sort(str8));
	printf("------------------------------------- \n\n");
}
