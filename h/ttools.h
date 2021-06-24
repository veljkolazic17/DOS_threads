#ifndef _TTOOLS_H_
#define _TTOOLS_H_

//pretpostavljeni memorijski model: huge

#include <iostream.h>
#include <dos.h>
#include "thread.h"
#include "dThread.h"
#include "SCHEDULE.H"
#include "llist.h"


//pomocne promenljive za prekid tajmera

// Zabranjuje prekide
#define lock asm cli

// Dozvoljava prekide
#define unlock asm sti

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

#endif