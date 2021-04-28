#include "../h/ttools.h"
#include "../h/PCB.h"
#include "../h/iterator.h"
#include "../h/ksem.h"

List* KernelSem::KernelSemList = new List();


unsigned List::removeAtSem(unsigned int id) volatile{//moralo je ovde da ne bi doslo do problema oko h fajlova
    Node* iter = first;
    while(iter != 0){
      KernelSem* tdata = (KernelSem*)(iter->data);
      if(tdata->id == id){
		if(current == iter)
		{
			if(current->next!=NULL)
				current = current->next;
			else
				current = current->last;
		}
        Node* temp = iter;
        last->next = iter->next;
        last->next->last = last;
        delete iter;
        return 1;
      } 
      iter = iter->next;
    }
    return 2;
}

void KernelSem::block(unsigned int time){
    lock
    this->blokirane->putNext((PCB*)PCB::running);
    PCB::running->blokirana = 1;
    PCB::running->waitTime=time;
    unlock
    dispatch();
}

void KernelSem::unblock(){
    lock
    Iterator* iterator = new Iterator(this->blokirane);
    if(this->blokirane->first == NULL)
        return;
    PCB* pcbCur = (PCB*)iterator->currentData();
    this->blokirane->removeAtPCB(pcbCur->id);
    pcbCur->blokirana = 0;
    pcbCur->returnValue=1;
    pcbCur->waitTime=0;
    unlock
    dispatch();
}

void KernelSem::unblockSelected(PCB* pcbCur){
    lock
    if(this->blokirane->first == NULL)
        return;
    
    this->blokirane->removeAtPCB(pcbCur->id);
    pcbCur->blokirana = 0;
    pcbCur->returnValue=-1;
    unlock
    dispatch();
}

int KernelSem::wait(unsigned int time){
    lock
    if(--(this->value) < 0) this->block(time);
    unlock
    return PCB::running->returnValue;
}

void KernelSem::signal(){
    lock
    if(++(this->value) <= 0) this->unblock();
    unlock
}

KernelSem::~KernelSem(){
    lock
    KernelSem::KernelSemList->removeAtSem(this->id);
    unlock
}

void KernelSem::tickSemaphore(){
    Iterator* iterator = new Iterator(KernelSem::KernelSemList);
    Iterator* iterator2 = new Iterator(KernelSem::KernelSemList);
    KernelSem* iter = NULL;
    while((iter = (KernelSem*)iterator->iterateNext()) != NULL){
        PCB* iter2=NULL;
        iterator2->iteratorReset();
        while((iter2=(PCB*)iterator2->iterateNext()) != NULL){
            if(--(iter2->waitTime)==0){
                iter->unblockSelected(iter2);
            }
        }
    }



}