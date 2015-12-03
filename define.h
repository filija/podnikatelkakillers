#define ID 			0	// nazev promenne
#define INT_V	 	1	// hodnota o typu int
#define DOUBLE_V	2
#define STRING_V	3

#define LS_ZAVORKA  5  // '{'
#define PS_ZAVORKA	6  // '}'
#define L_ZAVORKA	7  // '('
#define P_ZAVORKA	8  // ')'

#define PLUS		9  // '+'
#define MINUS		10 // '-'
#define DELENI		11 // '/'
#define NASOBENI	12 // '*'

#define CARKA			4  // ','
#define STREDNIK		13 // ';'
#define ROVNA_SE		14 // '=='
#define MENE_NEBO_ROVNO	15 // '<='
#define MENE 			16 // '<'
#define NEROVNOST		17 // '!='
#define VICE_NEBO_ROVNO	18 // '>='
#define VICE 			19 // '>'
#define PRIRAZENI		20 // '='
#define CTENI			21 // '>>'	
#define ZAPIS			22 // '<<'

#define AUTO 			23 //klicova slova
#define CIN 			24
#define COUT			25
#define DOUBLE 			26
#define ELSE			27
#define FOR				28
#define IF				29
#define INT 			30
#define RETURN			31
#define STRING 			32
#define MAIN			33

#define FILEEND			34

#define LEX_ERR			-1
#define SYN_ERR			-2
#define SEM_ERR			-3
#define STYPE_ERR		-4
#define AUTO_ERR		-5
#define SOTHERS_ERR		-6 // jina sem chyba
#define CIN_ERR			-7
#define NONINC_ERR		-8 // neinicializovana promenna
#define DIVZERO_ERR		-9 // deleni nulou
#define ROTHERS_ERR		-10 // ostatni behove chyby
#define INTERNAL_ERR	-99
#define IS_OK			0 // all okey

#define SHIFT			1 // shift
#define RPLCE			2 // shift
#define HADLE			3 // handle
#define SHERR 			4 // chyba v precedencce

#define S_ID			0
#define S_L_ZAVORKA		1
#define S_P_ZAVORKA		2
#define S_PLUS			3
#define S_MINUS			4
#define S_NASOBENI		5
#define S_DELENI		6
#define S_ROVNA_SE		7
#define S_MENE_NEBO_ROVNO	8
#define S_MENE 			9
#define S_NEROVNOST		10
#define S_VICE_NEBO_ROVNO	11
#define S_VICE 			12
#define S_DOLAR			13