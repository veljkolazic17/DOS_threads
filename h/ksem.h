#ifndef _KSEM_H_
#define _KSEM_H_
#include "thread.h"
#include "llist.h"


// treba negde da se inicijalizuje !!!

class KernelSem{
public:
    int value;
    List* blokirane;
    ID id;
    static List* KernelSemList;

    KernelSem(int value){
        this->value = value;
        blokirane = new List(); 
        KernelSemList->putNext(this);
        // kernel sem izvadi iz liste
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