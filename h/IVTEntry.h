#ifndef _ivtentry_h_
#define _ivtentry_h_
#include "event.h"

typedef void interrupt (*pInterrupt)(...);


class IVTEntry {

public:

    static IVTEntry* get_entry(IVTNo ivtNo);

    IVTEntry(IVTNo ivtNo, pInterrupt intr);

    void init_kernel(KernelEv* myKernelEv);
    void intr_old();
    void signal_ivt();
    void set_myKernelEv(KernelEv* myKernelEv);

    ~IVTEntry();

    KernelEv* myKernelEv;
    IVTNo entryNumber;
    pInterrupt oldIntr;
    pInterrupt newIntr;
};
#endif
