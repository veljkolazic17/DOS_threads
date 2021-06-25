#include "../h/ttools.h"
#include "../h/PCB.h"
#include "../h/iterator.h"
#include "../h/ksem.h"
#include<stdio.h>

List* KernelSem::KernelSemList = new List();
ID KernelSem::idS = 0;

unsigned List::removeAtSem(unsigned int id) volatile{//moralo je ovde da ne bi doslo do problema oko h fajlova
    Node* iter = first;
    while(iter != 0){
      KernelSem* tdata = (KernelSem*)(iter->data);
      if(tdata->id == id){
        Node* temp = iter;
		if(iter == first){
			first = iter->next;
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

void KernelSem::block(unsigned int time){
	if(time > 0)
		Shared::brojSemBlokiranih++;
    PCB::running->blokirana = 1;
    PCB::running->waitTime=time;
    this->blokirane->putNext((PCB*)PCB::running);
    //DOCS samo je ovde potreban dispatch posto se nit mora odmah blokirati
    dispatch();
}

void KernelSem::unblock(){
    Iterator* iterator = new Iterator(this->blokirane);
    PCB* pcbIter = (PCB*)iterator->currentData();
    if(pcbIter == NULL) return; // dodato kao mnogo bitno
    Shared::brojSemBlokiranih--;
    this->blokirane->removeAtPCB(pcbIter->id);
    pcbIter->blokirana = 0;
    pcbIter->returnValue=1;
    pcbIter->waitTime=0;
    Scheduler::put(pcbIter);
}

void KernelSem::unblockSelected(PCB* pcbCur){
    Shared::brojSemBlokiranih--;
    this->blokirane->removeAtPCB(pcbCur->id);
    pcbCur->blokirana = 0;
    pcbCur->returnValue=0;
    pcbCur->waitTime=0;
    //DOCS vraca kontekst ali kada to bude bilo potrebno
    Scheduler::put((PCB*)pcbCur);
}

int KernelSem::wait(unsigned int time){
    lockf();
    if(--(this->value) < 0) this->block(time);
    unlockf();
    return PCB::running->returnValue;
}

void KernelSem::signal(){
    lockf();
    if(++(this->value) <= 0) this->unblock();
    unlockf();
}

KernelSem::~KernelSem(){
    Iterator* semaphores = new Iterator(this->blokirane);
    PCB* pcb = NULL;
    while((pcb = (PCB*)semaphores->iterateNext()) != NULL){
    	this->unblockSelected(pcb);
    }

    KernelSem::KernelSemList->removeAtSem(this->id);
}

void KernelSem::tickSemaphore(){
    if(!Shared::brojSemBlokiranih) return;
    Iterator* semaphore_iterator = new Iterator(KernelSem::KernelSemList);
    KernelSem* semaphore = NULL;
    while((semaphore = (KernelSem*)semaphore_iterator->iterateNext())!= NULL){
        PCB* pcb = NULL;
        Iterator* pcb_iterator = new Iterator(semaphore->blokirane);
        while((pcb = (PCB*)pcb_iterator->iterateNext())!=NULL){
            if(pcb->waitTime == 0)continue;
            if(--(pcb->waitTime)==0){
                semaphore->unblockSelected(pcb);
            }
        }
    }
}
