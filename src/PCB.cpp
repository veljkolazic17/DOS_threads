#include "../h/PCB.h"
#include "../h/shared.h"
#include "../h/iterator.h"

volatile PCB* PCB::defaultPCB = 0;
volatile PCB* PCB::running = 0;
volatile PCB* PCB::mainPCB = 0;

PCB::PCB(StackSize stackSize, Time timeSlice,Thread* myThread){
		this->startovana=0;
		this->stackSize = stackSize;
		this->kvant = timeSlice*55;
		Shared::lockFlag = 0;
		Shared::lista->putNext(this);
		Shared::lockFlag = 1;
		this->myThread = myThread;
		waitList = new List();
		waitTime=0;
		returnValue=0;
	}

//Pazi ovde da se ne zajebes koristio si unsigned int a ne ID sto je isto al pazi 
unsigned List::removeAtPCB(unsigned int id) volatile{//moralo je ovde da ne bi doslo do problema oko h fajlova
    Node* iter = first;
    while(iter != 0){
      PCB* tdata = (PCB*)(iter->data);
      if(tdata->myThread->getId() == id){
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

void PCB::initDefault(){
    Shared::zahtevana_promena_konteksta = 0;
    if(PCB::defaultPCB != 0)
		return;
		Shared::lockFlag = 0;
		Shared::defaultThread = new DefaultThread();
		Shared::lista = new List();
		//lockFlag = 0; ne mora lock
		PCB::createProcess(Shared::defaultThread->myPCB);
		//lockFlag = 1;
		PCB::defaultPCB = Shared::defaultThread->myPCB;

		mainPCB = new PCB(4096,2,0);
		Shared::lista->putNext((PCB*)mainPCB);
		mainPCB->zavrsio = 0;
		mainPCB->blokirana = 0;
		PCB::running = mainPCB;
		Shared::brojac = 1;
		Shared::lockFlag = 1;
}

void PCB::exitThread(){
		Shared::lockFlag = 0;
		Iterator* iterator = new Iterator(PCB::running->waitList);
		PCB::running->zavrsio = 1;
		PCB* dtemp;
		while((dtemp = (PCB*)iterator->iterateNext())!=NULL){
			dtemp->blokirana = 0;
			if(dtemp->zavrsio == 0){
				Scheduler::put(dtemp);
			}
		}
		Shared::lockFlag = 1;
		dispatch();
}	

void PCB::run(){
	if(PCB::running->myThread){
		PCB::running->myThread->run();
		PCB::exitThread();  
		dispatch();
	}
}

void PCB::createProcess(PCB *newPCB){
		Shared::lockFlag = 0;
		unsigned stackSize=newPCB->stackSize;
		stackSize/=sizeof(unsigned);
		unsigned* st1 = new unsigned[stackSize];
		newPCB->stack =st1;
		st1[stackSize-1] =0x200;//setovan I fleg u pocetnom PSW-u za nit
		st1[stackSize-2] = FP_SEG(PCB::run); //ovo je zbog huge memorijskog modela
		st1[stackSize-3] = FP_OFF(PCB::run);

		newPCB->sp = FP_OFF(st1+stackSize-12); //svi sacuvani registri pri ulasku u interrupt rutinu
		newPCB->ss = FP_SEG(st1+stackSize-12);
		newPCB->bp = FP_OFF(st1+stackSize-12); //zasto je ovde +1012 ???
		newPCB->zavrsio = 0;
		newPCB->blokirana = 0;
		Shared::lockFlag = 1;
	}