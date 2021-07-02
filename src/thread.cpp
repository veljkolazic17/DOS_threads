#include "../h/thread.h"
#include "../h/PCB.h"
#include "../h/shared.h"
#include "../h/semaphor.h"
#include "../h/iterator.h"
#include "../h/ForkData.h"
#include<dos.h>
ID Thread::getRunningId(){
    return PCB::running->id;
}

Thread * Thread::getThreadById(ID id){
	lockf();
    Iterator* iterator = new Iterator((List*)Shared::lista);
    PCB* pcb = NULL;
    while((pcb = (PCB*)iterator->iterateNext()) != NULL){
        if(pcb->id == id){
            return pcb->myThread;
        }
    }
    delete iterator;
    unlockf();
    return NULL;
}

ID Thread::getId(){
    return this->myPCB->id;
}

Thread::Thread(StackSize stackSize, Time timeSlice){
	lockf();
    this->myPCB = new PCB(stackSize,timeSlice,this);
    if(myPCB){
    	int res = PCB::createProcess(this->myPCB);
    	if(res == -1 || !myPCB->waitList || !myPCB->deca){
    		handleErrors('+');
    	}
    }
    else{
    	handleErrors('+');
    }
	unlockf();
}

Thread::~Thread(){
	lockf();
    Shared::lista->removeAtPCB(this->getId());
    delete myPCB;
    unlockf();
}

void Thread::waitToComplete(){
    if (!this->myPCB->zavrsio && !PCB::running->zavrsio){ //TODO proveri kako je bilo
    	lockf();
    	PCB::running->blokirana = 1;
        this->myPCB->waitList->putNext((PCB*)PCB::running);
        dispatch();
        unlockf();
    }
}

void Thread::start(){
    if(this->myPCB->startovana == 0){
    	lockf();
		this->myPCB->startovana = 1;
		Shared::lista->putNext(this->myPCB);
		Scheduler::put(this->myPCB);
		unlockf();
    }
}

unsigned errorCountBefore,errorCountAfter;

ID Thread::fork(){
//	Shared::lockFlag++;
	/*
	 * prvobitno nije bilo locka ali posto clone radi new trebalo bi
	 * da se lockuje
	 */
	locknest
	errorCountBefore = Shared::errorCount;
	ForkData::deteThread= PCB::running->myThread->clone();
	errorCountAfter = Shared::errorCount;
	if(errorCountBefore != errorCountAfter){
		unlocknest
		return -1;
	}
	ForkData::detePCB = ForkData::deteThread->myPCB;
	ForkData::snap();
	int res = 0;
	if(PCB::running->original){
		ForkData::detePCB->original = 0;

		void* addr = MK_FP(ForkData::ss_,ForkData::sp_);
		unsigned int slen = PCB::running->stack + PCB::running->stackSize/sizeof(unsigned) - (unsigned int*)addr;
		unsigned int* local = ForkData::detePCB->stack+ForkData::detePCB->stackSize/sizeof(unsigned) - slen;
		unsigned diff = (unsigned)PCB::running->stack - (unsigned)ForkData::detePCB->stack;

		ForkData::detePCB->bp = ForkData::bp_-diff;
		ForkData::detePCB->ss = FP_SEG(local);
		ForkData::detePCB->sp = FP_OFF(local);

		PCB::running->deca->putNext(ForkData::detePCB);
		if(PCB::copyPCB(ForkData::detePCB,(PCB*)PCB::running) == -1){
			//TODO pitanje da li linija ispod treba da stoji
			delete ForkData::detePCB;
			unlocknest
			return -1;
		}
		Scheduler::put(ForkData::detePCB);
		//unlockf();
		/*Menjao sam da ne koristim vise unlockova
		* vec samo jedan
		* */
		//unlocknest
		res =  ForkData::detePCB->id;
	}else{
		PCB::running->original = 1;
		//unlockf();
		//TODO proveri ovo ispod
		/*Menjao sam da ne koristim vise unlockova
		 * vec samo jedan
		 * */
		//unlocknest
	}
//	Shared::lockFlag--;
	unlocknest
	return res;
}

//TODO ovde je prvo bilo obicno lock a ne locknest
void Thread::waitForForkChildren(){
	Iterator* children_iterator = new Iterator(PCB::running->deca);
	PCB* pcb = NULL;
	while((pcb = (PCB*)children_iterator->iterateNext()) != NULL){
		pcb->myThread->waitToComplete();
	}
	delete children_iterator;
}

void Thread::exit(){
	lockf();
	PCB::running->exitThread();
	unlockf();
}

