#ifndef _SHARED_H_
#define _SHARED_H_

class Thread;
class List;
class IVTEntry;

/*
    U ovomo fajlu ce se cuvati sve globalne promenljive ili promenljive 
    sa nepoznatom kategorijom.
*/

class Shared{
public:
    static volatile unsigned int lockFlag;//lockovanje zone bez ukidanja prekida
    static volatile int brojac;
    static volatile int zahtevana_promena_konteksta;
    static volatile unsigned int brojSemBlokiranih;
    static volatile Thread* defaultThread;
    static volatile List* lista;//lista svih aktivnih PCB-ova
    static volatile List* IVTEntries;
    static volatile unsigned int errorCount;
};
#endif


