#ifndef _TTOOLS_H_
#define _TTOOLS_H_

//pretpostavljeni memorijski model: huge

#define PREPAREENTRY(ivtNo, old)\
void interrupt froutine##ivtNo(...); \
IVTEntry inter_entry##ivtNo(ivtNo, froutine##ivtNo); \
void interrupt froutine##ivtNo(...) {\
	inter_entry##ivtNo.signal_ivt();\
	if (old == 1)\
		inter_entry##ivtNo.intr_old();\
}



#include <iostream.h>
#include <dos.h>
#include "thread.h"
#include "dThread.h"
#include "SCHEDULE.H"
#include "llist.h"
#include "../h/IVTEntry.h"

//pomocne promenljive za prekid tajmera

// Zabranjuje prekide
#define lock asm cli

// Dozvoljava prekide
#define unlock asm sti

//nestovani prekidi
#define locknest asm pushf;\
	asm cli;
#define unlocknest asm popf;


void initDefaultWrapper();

void interrupt timer();

void dispatch();

// postavlja novu prekidnu rutinu
void inic();

// vraca staru prekidnu rutinu
void restore();

//funkcije za lockovanje
void lockf();
void unlockf();

unsigned int handleErrors(char c);


#endif
