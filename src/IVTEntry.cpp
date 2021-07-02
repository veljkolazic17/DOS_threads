#include "../h/IVTEntry.h"
#include<DOS.H>
#include "llist.h"
#include "../h/iterator.h"
#include "../h/kernelEv.h"
#include "../h/Shared.h"
#include <stdio.h>
#include "../h/ttools.h"

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt intr){
	asm pushf;
	asm cli;
	this->oldIntr = 0;
	this->newIntr = 0;
    if(ivtNo != 0x8 && ivtNo != 0x60 && ivtNo <= 255){
		this->entryNumber = ivtNo;
		this->oldIntr = getvect(ivtNo);
		setvect(ivtNo,intr);
		if(Shared::IVTEntries == 0)
			Shared::IVTEntries = new List();
        Shared::IVTEntries->putNext(this);
    }
    asm popf;
}

IVTEntry::~IVTEntry(){
    if(this->oldIntr){
        setvect(this->entryNumber,oldIntr);
        //Shared::IVTEntries->removeAtIVT(this->entryNumber);
        this->intr_old();
    }
}

void IVTEntry::intr_old(){
	asm cli;
    if(this->oldIntr)
    	(*oldIntr)();
    asm sti;
}

void IVTEntry::init_kernel(KernelEv* myKernelEv){
    this->myKernelEv = myKernelEv;
}

unsigned List::removeAtIVT(IVTNo ivtNo) volatile{
    Node* iter = first;
    while(iter != 0){
      IVTEntry* tdata = (IVTEntry*)(iter->data);
      if(tdata->entryNumber == ivtNo){
        Node* temp = iter;
		if(iter == first){
			first = iter->next;
			first->last = NULL;
		}
		else if(iter == last){
			last = iter->last;
			last->next = NULL;
		}
		else{
			temp = iter->last;
			iter->last->next = iter->next;
			iter->next->last = iter->last;
		}
        delete iter;
		this->length--;
        return 1;
      } 
      iter = iter->next;
    }
    return 2;
}

IVTEntry* IVTEntry::get_entry(IVTNo ivtNo){
    if(ivtNo > 255) return NULL;
    lockf();
    Iterator* iterator = new Iterator((List*)Shared::IVTEntries);
    IVTEntry* ivt = NULL;
    while((ivt = (IVTEntry*)iterator->iterateNext())!= NULL){
        if(ivt->entryNumber == ivtNo){
            break;
        }
    }
    asm pushf;
    asm cli;
    delete iterator;
    asm popf;
    unlockf();
    return ivt;
}
void IVTEntry::signal_ivt(){
    //TODO proveri treba li da se proveri da li je myKernelEv == 0
	if(myKernelEv)
		myKernelEv->signal();

	//TODO mozda mora, a mozda i ne
	else{
		if(this->oldIntr){
			locknest
			setvect(this->entryNumber,this->oldIntr);
			Shared::IVTEntries->removeAtIVT(this->entryNumber);
			unlocknest
		}
	}
}

void IVTEntry::set_myKernelEv(KernelEv* myKernelEv){
    this->myKernelEv = myKernelEv;
}
