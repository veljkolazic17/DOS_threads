#include "../h/thread.h"
#include "../h/PCB.h"
#include "../h/shared.h"
#include "../h/semaphor.h"
#include "../h/iterator.h"


ID Thread::getRunningId(){
    return PCB::running->id;
}

Thread * Thread::getThreadById(ID id){
    Iterator* iterator = new Iterator((List*)Shared::lista);
    PCB* pcb = NULL;
    while((pcb = (PCB*)iterator->iterateNext()) != NULL){
        if(pcb->id == id){
            return pcb->myThread;
        }
    }
    return NULL;
}

ID Thread::getId(){
    return this->myPCB->id;
}

Thread::Thread(StackSize stackSize, Time timeSlice){
    this->myPCB = new PCB(stackSize,timeSlice,this);
}

Thread::~Thread(){
	lockf();
    Shared::lista->removeAtPCB(this->getId());
    delete myPCB;
    unlockf();
}

void Thread::waitToComplete(){
    if (!this->myPCB->zavrsio){
        this->myPCB->waitList->putNext((PCB*)PCB::running);
        PCB::running->blokirana = 1;
        dispatch();
    }
}

void Thread::start(){
    if(this->myPCB->startovana == 1)
        return;
    int allocated = PCB::createProcess(this->myPCB);

    if(allocated == -1) return;

    this->myPCB->startovana = 1;
    Scheduler::put(this->myPCB);
}
