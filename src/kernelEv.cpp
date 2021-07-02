#include "../h/kernelEv.h"
#include "../h/event.h"
#include "../h/ttools.h"
#include "../h/IVTEntry.h"
#include "../h/PCB.h"
#include <stdio.h>
KernelEv::KernelEv(IVTNo ivtNo){
	lockf();
    this->myIVTEntry = IVTEntry::get_entry(ivtNo);
    this->blocked = 0;
    this->val = 0;
    this->owner = (PCB*)PCB::running;
    if(this->myIVTEntry){
        this->myIVTEntry->set_myKernelEv(this);
    }
    unlockf();
}
void KernelEv::wait(){
	lockf();
    if(this->owner != (PCB*)PCB::running || this->myIVTEntry == 0) return;
    if(this->val == 0){
        this->blocked = 1;
        PCB::running->blokirana = 1;
        dispatch();
    }
    else{
        this->val = 0;
    }
    unlockf();
}

void KernelEv::signal(){
	lockf();
        if(this->blocked == 0){
            this->val = 1;
        }
        else{
            this->owner->blokirana = 0;
            this->blocked = 0;
            if(this->owner->zavrsio == 0)
            	Scheduler::put(this->owner);
            dispatch();
        }
     unlockf();
}

KernelEv::~KernelEv(){
	signal();
	locknest
	if(this->myIVTEntry){
		this->myIVTEntry->set_myKernelEv(0);
	}
	unlocknest
}
