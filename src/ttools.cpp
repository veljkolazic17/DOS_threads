#include "../h/PCB.h"
#include "../h/shared.h"
#include "../h/ksem.h"
#include "../h/llist.h"

unsigned tsp;
unsigned tss;
unsigned tbp;

extern void tick();

void initDefaultWrapper(){
	PCB::initDefault();
}

unsigned int handleErrors(char c){
	if(c == '+'){
		Shared::errorCount++;
	}
	else if(c == '-'){
		if(Shared::errorCount >= 0){
			Shared::errorCount--;
		}
	}
	return Shared::errorCount;
}


void interrupt timer(){	// prekidna rutina



	if(Shared::zahtevana_promena_konteksta == 0){
		KernelSem::tickSemaphore();
		tick();
	}

	if(!Shared::zahtevana_promena_konteksta && PCB::running->neograniceno == 1){
//		if(Shared::lockFlag <= 0){
			asm int 60h;
			return;
//		}
	}
	else if (!Shared::zahtevana_promena_konteksta && PCB::running->neograniceno == 0){
		PCB::running->brojac--;
	}

	//RADI PROMENU KONTEKSTA
	if (PCB::running->brojac == 0 || Shared::zahtevana_promena_konteksta) {
		if(Shared::lockFlag <= 0){
		Shared::zahtevana_promena_konteksta = 0;
		//TODO treba proveriti da li treba svaki put kad se radi dispatch da se menja kontekst

		asm {
			// cuva sp
			mov tsp, sp
			mov tss, ss
			mov tbp, bp //dodatos
		}
	
		PCB::running->sp = tsp;
		PCB::running->ss = tss;
		PCB::running->bp = tbp;


		if(PCB::running && !PCB::running->zavrsio &&
		   !PCB::running->blokirana &&
		   PCB::running!=PCB::defaultPCB){
			Scheduler::put((PCB*)PCB::running); //mora da se kastuje zato sto je running volatile
		   }

		/*
		 * PROVERA DA LI JE DOSLO DO GRESKE PRILIKOM KREIRANJA
		 */

		if(!PCB::running){
			handleErrors('-');
		}

		PCB::running= Scheduler::get();

		if(PCB::running == 0){
			PCB::running = PCB::defaultPCB;
		}

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;

		PCB::running->brojac = PCB::running->kvant;

		asm {
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp,tbp //dodato
		}
	}else{
		Shared::zahtevana_promena_konteksta = 1;
	}
	}

	if(!Shared::zahtevana_promena_konteksta) asm int 60h;
	
}

void dispatch(){ // sinhrona promena konteksta
	locknest
	Shared::zahtevana_promena_konteksta = 1;
	timer();
	unlocknest
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

//funkcije za lockovanje pomocu lockFlag-a
void lockf(){
	Shared::lockFlag++;
}

void unlockf(){
	Shared::lockFlag--;
	if(Shared::zahtevana_promena_konteksta && !Shared::lockFlag){
		dispatch();
	}
}



