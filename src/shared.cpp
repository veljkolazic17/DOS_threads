#include "../h/shared.h"
#include <stdlib.h>


volatile int Shared::brojac = 0;
volatile int Shared::zahtevana_promena_konteksta = 0;
volatile unsigned int Shared::lockFlag = 0;
volatile unsigned int Shared::brojSemBlokiranih = 0;
volatile Thread* Shared::defaultThread = NULL;
volatile List* Shared::lista = NULL;
volatile List* Shared::IVTEntries = NULL;
volatile unsigned int Shared::errorCount = 0;


