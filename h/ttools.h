#ifndef _TTOOLS_H_
#define _TTOOLS_H_

//pretpostavljeni memorijski model: huge

#include <iostream.h>
#include <dos.h>
#include "thread.h"
#include "dThread.h"
#include "SCHEDULE.H"
#include "llist.h"

volatile Thread* defaultThread;

void exitThread();

//pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;

volatile unsigned int lockFlag = 1;//lockovanje zone bez ukidanja prekida
volatile int brojac = 20;
volatile int zahtevana_promena_konteksta = 0;

// Zabranjuje prekide
#define lock asm cli

// Dozvoljava prekide
#define unlock asm sti

volatile List* lista;
class PCB;

class PCB{
public:
	static volatile PCB* defaultPCB;
	static volatile PCB* running;
	static volatile PCB* mainPCB; 
	unsigned bp; //kako bi mogli da koristimo lokalne promenljive
	unsigned sp; //16 bitni registri preko kojih odbijamo 28bitnu adresu
	unsigned ss;
	unsigned* stack;
	unsigned blokirana;
	unsigned zavrsio;
	unsigned stackSize;
	unsigned startovana;
	Thread* myThread;
	List* waitList;
	ID id;
	int kvant;

	PCB(StackSize stackSize, Time timeSlice,Thread* myThread){
		this->startovana=0;
		this->stackSize = stackSize;
		this->kvant = timeSlice*55;
		lockFlag = 0;
		lista->putNext(this);
		lockFlag = 1;
		this->myThread = myThread;
		waitList = new List();
	}

	static void run();	//ovo je metoda wrapper

	static void initDefault(){
	if(PCB::defaultPCB != 0)
		return;
		lockFlag = 0;
		defaultThread = new DefaultThread();
		lista = new List();
		//lockFlag = 0; ne mora lock
		PCB::createProcess(defaultThread->myPCB);
		//lockFlag = 1;
		PCB::defaultPCB = defaultThread->myPCB;

		mainPCB = new PCB(4096,2,0);
		lista->putNext((PCB*)mainPCB);
		mainPCB->zavrsio = 0;
		mainPCB->blokirana = 0;
		PCB::running = mainPCB;
		brojac = 1;
		lockFlag = 1;
	}

	static void createProcess(PCB *newPCB){
		lockFlag = 0;
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
		lockFlag = 1;
	}

	static void exitThread();
};

void initDefaultWrapper(){
	lock
	PCB::initDefault();
	unlock
}

volatile PCB* PCB::defaultPCB = 0;
volatile PCB* PCB::running = 0;
volatile PCB* PCB::mainPCB = 0;

unsigned List::removeAtPCB(ID id) volatile{//moralo je ovde da ne bi doslo do problema oko h fajlova
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

void PCB::exitThread(){
		lockFlag = 0;
		PCB::running->zavrsio = 1;
		PCB* dtemp;
		while((dtemp = (PCB*)PCB::running->waitList->iterateNext())!=NULL){
			dtemp->blokirana = 0;
			if(dtemp->zavrsio == 0){
				Scheduler::put(dtemp);
			}
		}
		lockFlag = 1;
		dispatch();
}	


void PCB::run(){
	if(PCB::running->myThread){
		PCB::running->myThread->run();
		PCB::exitThread();  
		dispatch();
	}
}

void interrupt timer(){	// prekidna rutina
	if (!zahtevana_promena_konteksta) brojac--;
	if (brojac == 0 || zahtevana_promena_konteksta) {
		if(lockFlag == 0){
			zahtevana_promena_konteksta = 1;
			return;
		}
		zahtevana_promena_konteksta = 0;
		asm {
			// cuva sp
			mov tsp, sp
			mov tss, ss
			mov tbp, bp //dodatos
		}
	
		PCB::running->sp = tsp;
		PCB::running->ss = tss;
		PCB::running->bp = tbp;

		
		if(!PCB::running->zavrsio && 
		   !PCB::running->blokirana &&
		   PCB::running!=PCB::defaultPCB){
			cout << "dao: " <<(void*)PCB::running << endl;
			Scheduler::put((PCB*)PCB::running); //mora da se kastuje zato sto je running volotile
		   }
				
		PCB::running= Scheduler::get();
		cout << "dobio: " <<(void*)PCB::running << endl;
		

		if(PCB::running == 0){
			cout << "blokirano sve" << endl;
			PCB::running = PCB::defaultPCB;
		}
		

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;

		brojac = PCB::running->kvant;

		asm {
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp,tbp //dodato
		}
	}

	if(!zahtevana_promena_konteksta) asm int 60h;
	zahtevana_promena_konteksta = 0;
}

void dispatch(){ // sinhrona promena konteksta
	lock
	zahtevana_promena_konteksta = 1;
	timer();
	unlock
}

unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina

// postavlja novu prekidnu rutinu
void inic(){
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

// vraca staru prekidnu rutinu
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax


		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
}


#endif