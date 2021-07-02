//File: kernelEv.h
#ifndef _kernelev_h_
#define _kernelev_h_
#include "event.h"
class IVTEntry;
class PCB;

class KernelEv{
public:
    KernelEv(IVTNo ivtNo);
    void wait();
    void signal();
    ~KernelEv();
private:
    int blocked;
    IVTEntry* myIVTEntry;
    unsigned val;
    PCB* owner;

};
#endif
