#include "../h/PCB.h"
#include "../h/shared.h"
#include "../h/ksem.h"


unsigned tsp;
unsigned tss;
unsigned tbp;

void initDefaultWrapper(){
	lock
	PCB::initDefault();
	unlock
}

void interrupt timer(){	// prekidna rutina
	if (!Shared::zahtevana_promena_konteksta) Shared::brojac--;
	if (Shared::brojac == 0 || Shared::zahtevana_promena_konteksta) {
		if(Shared::lockFlag == 0){
			Shared::zahtevana_promena_konteksta = 1;
			return;
		}
		Shared::zahtevana_promena_konteksta = 0;
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
			//cout << "dao: "<< PCB::running->ime <<(void*)PCB::running << endl;
			Scheduler::put((PCB*)PCB::running); //mora da se kastuje zato sto je running volotile
		   }
				
		PCB::running= Scheduler::get();
		//cout << "dobio: "<< PCB::running->ime <<(void*)PCB::running << endl;
		

		if(PCB::running == 0){
			//cout << "blokirano sve" << endl;
			PCB::running = PCB::defaultPCB;
		}
		

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;

		Shared::brojac = PCB::running->kvant;

		asm {
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp,tbp //dodato
		}
	}

	KernelSem::tickSemaphore();

	if(!Shared::zahtevana_promena_konteksta) asm int 60h;
	Shared::zahtevana_promena_konteksta = 0;
}

void dispatch(){ // sinhrona promena konteksta
	lock
	Shared::zahtevana_promena_konteksta = 1;
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