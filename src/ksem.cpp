#include "../h/ttools.h"
#include "../h/PCB.h"
#include "../h/iterator.h"
#include "../h/ksem.h"

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
    lock
    PCB::running->blokirana = 1;
    PCB::running->waitTime=time/55;
    this->blokirane->putNext((PCB*)PCB::running);
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
    Scheduler::put(pcbCur);
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
    pcbCur->waitTime=0;
    Scheduler::put((PCB*)pcbCur);
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