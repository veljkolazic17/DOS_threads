#include "../h/PCB.h"
#include "../h/shared.h"
#include "../h/iterator.h"

//inicijalizacija statickih
volatile PCB* PCB::defaultPCB = 0;
volatile PCB* PCB::running = 0;
volatile PCB* PCB::mainPCB = 0;
ID PCB::idS = 0;

PCB::PCB(StackSize stackSize, Time timeSlice,Thread* myThread){
		this->id = PCB::idS++;
		this->startovana=0;
		if(stackSize > 65535){
			stackSize = 65535;
		}
		this->stackSize = stackSize;
		this->kvant = timeSlice*55;

		this->myThread = myThread;
		waitList = new List();
		waitTime=0;
		returnValue=-1;

		//PROVERA DA LI JE U PITANJU NEOGRANICENI REZIM
		if(timeSlice == 0){
			this->neograniceno = 1;
		}
		else{
			
			this->neograniceno = 0;
		}
		this->brojac = timeSlice;
		
	}

//Pazi ovde da se ne zajebes koristio si unsigned int a ne ID sto je isto al pazi 
unsigned List::removeAtPCB(unsigned int id) volatile{//moralo je ovde da ne bi doslo do problema oko h fajlova
    Node* iter = first;
    while(iter != 0){
      PCB* tdata = (PCB*)(iter->data);
      if(tdata->id == id){
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

void PCB::initDefault(){
    Shared::zahtevana_promena_konteksta = 0;
    if(PCB::defaultPCB != 0)
		return;
		Shared::defaultThread = new DefaultThread();
		Shared::lista = new List();
		PCB::createProcess(Shared::defaultThread->myPCB);
		PCB::defaultPCB = Shared::defaultThread->myPCB;

		mainPCB = new PCB(4096,2,0);
		mainPCB->zavrsio = 0;
		mainPCB->blokirana = 0;
		PCB::createProcess((PCB*)mainPCB);
		PCB::running = mainPCB;
}

void PCB::exitThread(){
		Iterator* iterator = new Iterator(PCB::running->waitList);
		PCB::running->zavrsio = 1;
		PCB::running->neograniceno = 0;
		PCB* dtemp;
		while((dtemp = (PCB*)iterator->iterateNext())!=NULL){
			dtemp->blokirana = 0;
			if(dtemp->zavrsio == 0){
				Scheduler::put(dtemp);
			}
		}		
		dispatch();
}	

void PCB::run(){
	if(PCB::running->myThread){
		PCB::running->myThread->run();
		PCB::exitThread();  
	}
}

int PCB::createProcess(PCB *newPCB){
		unsigned stackSize=newPCB->stackSize;
		stackSize/=sizeof(unsigned);
		unsigned* st1 = new unsigned[stackSize];

		if(st1 == 0) return -1;

		newPCB->stack =st1;
		st1[stackSize-1] =0x200;//setovan I fleg u pocetnom PSW-u za nit
		st1[stackSize-2] = FP_SEG(PCB::run); //ovo je zbog huge memorijskog modela
		st1[stackSize-3] = FP_OFF(PCB::run);

		lock
		newPCB->sp = FP_OFF(st1+stackSize-12); //svi sacuvani registri pri ulasku u interrupt rutinu
		newPCB->ss = FP_SEG(st1+stackSize-12);
		newPCB->bp = FP_OFF(st1+stackSize-12); //zasto je ovde +1012 ???
		newPCB->zavrsio = 0;
		newPCB->blokirana = 0;
		unlock

		lock
		Shared::lista->putNext(newPCB);
		unlock
		return 1;
}
