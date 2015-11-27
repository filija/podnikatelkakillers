/*
*	IFJ Projekt
*		scanner pro jazyk IFJ15
*		autor: Nikolas Kantor
*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "define.h"
#include "scanner.h"

FILE *source;


void setSourceFile(FILE *f){
  source = f;
}

void lErr(){
	fprintf(stderr,"LEX_ERROR");
	exit(1);
}

int getNextToken(string *attr){

	int state = 0; //při hledání každého nového tokenu bude počáteční stav 0 
	int c;
	strClear(attr);//vyčítím řetězec pro nové použití

	while(1){
		c = getc(source);
    	switch (state){
			
			/***** DEFAULT STATE *****/
			case 0:
				if (isspace(c))
					state = 0;
				else if (c == '<')
					state = 1;
				else if (c == '>')
					state = 2;
				else if (c == '!')
					state = 3;
				else if (c == '=')
					state = 4;
				else if (isdigit(c)){
					strAddChar(attr, c);
					state = 5;
				}
				else if (isalpha(c) || c == '_'){
					strAddChar(attr, c);
					state = 11;
				}
				else if (c == '"'){
					strAddChar(attr, c);
					state = 15;
				}
				else if (c == '/')
					state = 20;
				else{
						switch(c){
							case ';':
								return STREDNIK;
							break;
							case '+':
								return PLUS;
							break;
							case '-':
								return MINUS;
							break;
							case '*':
								return NASOBENI;
							break;
							case ',':
								return CARKA;
							break;
							case '(':
								return L_ZAVORKA;
							break;
							case ')':
								return P_ZAVORKA;
							break;
							case '{':
								return LS_ZAVORKA;
							break;
							case '}':
								return PS_ZAVORKA;
							break;
							case EOF:
								return FILEEND;
							break;
							default:
								lErr();
							break;
						}
				}


			break;
			/***** LESS OR LESS_OR_EQUALS *****/
			case 1:
				if (c == '=')
    				return MENE_NEBO_ROVNO; // token -> [<=, ]
    			else if (c == '<')
    				return ZAPIS; // token -> [<<, ]
    			else {
    				ungetc(c, source);
    				return MENE; // token -> [<, ]
				}
			break;
			/***** MORE OR MORE_OR_EQUALS *****/
			case 2:
				if (c == '=')
    				return VICE_NEBO_ROVNO; // token -> [>=, ]
    			else if (c == '>')
    				return CTENI; // token -> [>>, ]
    			else {
    				ungetc(c, source);
    				return VICE; // token -> [>, ]
				}
			break;
			/***** NOT EQUALS *****/
			case 3:
				if (c == '=')
    				return NEROVNOST; // token -> [!=, ]
    			else
   					lErr();
			break;
			/***** EQUALS *****/
			case 4:
				if (c == '=')
    				return ROVNA_SE; // token -> [==, ]
    			else {
    				ungetc(c, source);
   					return PRIRAZENI;
				}
			break;

			/***** INT OR DOUBLE *****/
			case 5:
				if (isdigit(c))
    				strAddChar(attr, c);
    			else if (c == '.') {
    				strAddChar(attr, c);
    				state = 6;
    			}
    			else if (c == 'e' || c == 'E'){
    				strAddChar(attr, c);
    				state = 8;
    			}
    			else
    				ungetc(c, source);
    				return INT_V; // token -> [INT_V, N]
    		break;
    		case 6:
    			if (isdigit(c)){
    				strAddChar(attr, c);
    				state = 7;
    			}
    			else
    				lErr();
    		break;
    		case 7:
    			if (isdigit(c))
    				strAddChar(attr, c);
    			else if (c == 'e' || c == 'E'){
    				strAddChar(attr, c);
    				state = 8;
    			}
    			else
    				ungetc(c, source);
    				return DOUBLE_V; // token -> [DOUBLE_V, D.D]
    		break;
    		case 8:
    			if (isdigit(c)){
    				strAddChar(attr, c);
    				state = 10;
    			}
    			else if (c == '+' || c == '-'){
    				strAddChar(attr, c);
    				state = 9;
    			}
    			else
    				lErr();
    		break;
    		case 9:
    			if (isdigit(c)) {
    				strAddChar(attr, c);
    				state = 10;
    			}
    			else
    				lErr();
    		break;
    		case 10:
    			if (isdigit(c))
    				strAddChar(attr, c);
    			else
    				ungetc(c, source);
    				return DOUBLE_V;
    		break;

			/***** ID OR KEYWORD *****/
			case 11:
				if (isalnum(c) || c == '_')
					strAddChar(attr, c);
				else{
					ungetc(c, source);
					if (strCmpConstStr(attr, "auto"))
						return AUTO;
					else if (strCmpConstStr(attr, "cin"))
						return CIN;
					else if (strCmpConstStr(attr, "cout"))
						return COUT;
					else if (strCmpConstStr(attr, "double"))
						return DOUBLE;
					else if (strCmpConstStr(attr, "else"))
						return ELSE;
					else if (strCmpConstStr(attr, "for"))
						return FOR;
					else if (strCmpConstStr(attr, "if"))
						return IF;
					else if (strCmpConstStr(attr, "int"))
						return INT;
					else if (strCmpConstStr(attr, "return"))
						return RETURN;
					else if (strCmpConstStr(attr, "string"))
						return STRING;
					else
						return ID;
				}

			break;

			/***** STRING *****/
			case 15:
				if (c == '\\') {
					strAddChar(attr, c);
					state = 16;
				}
				else if (c == '"'){
					strAddChar(attr, c);
					return STRING_V;
				}
				else
					strAddChar(attr, c);
			break;
			case 16:
				strAddChar(attr, c);
				state = 15;
			break;

			/***** COMMENT *****/
			case 20:
				if (c == '*')
					state = 21;
				else if (c == '/')
					state = 23;
				else {
					ungetc(c, source);
					return DELENI;
				}
			break;
			case 21:
				if (c == '*')
					state = 22;
			break;
			case 22:
				if (c == '/')
					state = 0;
				else
					state = 21;
			break;
			case 23:
				if (c == '\\')
					state = 24;
			break;
			case 24:
				if (c == 'n')
					state = 0;
				else
					state = 23;
			break;
    	}
    }
}