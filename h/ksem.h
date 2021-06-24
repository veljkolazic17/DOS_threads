#ifndef _KSEM_H_
#define _KSEM_H_
#include "../h/thread.h"
#include "../h/llist.h"
#include "../h/ttools.h"


// treba negde da se inicijalizuje !!!

class KernelSem{
public:
    static ID idS;
    int value;
    List* blokirane;
    ID id;
    static List* KernelSemList;

    KernelSem(int value){
        KernelSem::idS++;
        this->id = KernelSem::idS;
        this->value = value;
        blokirane = new List(); 
        lockf();
        KernelSemList->putNext(this);
        unlockf();
    }
    
    int wait(Time time);
    void signal();
    void block(unsigned int time);
    void unblock();
    void unblockSelected(PCB* pcbCur);
    static void tickSemaphore();

    ~KernelSem();
};
#endif
