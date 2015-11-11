#define I_SCITANI		1
#define I_ODCITANI		2
#define I_NASOBENI		3
#define I_DELENI		4
#define I_MENSI			5
#define I_MENSI_NEBO	6
#define I_VETSI			7
#define I_VETSI_NEBO	8
#define I_ROVNO			9
#define I_NEROVNO		10
#define I_PRIRAZENI		11
#define I_SKOK			12
#define I_PODM_SKOK		13
#define I_LABEL			14
#define I_CTENI			15
#define I_ZAPIS			16


typedef struct{
  int instType;  // typ instrukce
  void *addr1; // adresa 1
  void *addr2; // adresa 2
  void *addr3; // adresa 3
} tInstr;