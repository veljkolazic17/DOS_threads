#ifndef _PCB_H_
#define _PCB_H_
#include "thread.h"
#include "llist.h"
#include "ttools.h"



class PCB{
public:
	//STATICKE PROMENJLIVE
	static ID idS;
	static volatile PCB* defaultPCB;
	static volatile PCB* running;
	static volatile PCB* mainPCB; 

	//STVARI POTREBNE ZA SAM PROCES
	unsigned bp; //kako bi mogli da koristimo lokalne promenljive
	unsigned sp; //16 bitni registri preko kojih odbijamo 28bitnu adresu
	unsigned ss;
	unsigned* stack;

	//STANJA
	unsigned blokirana;
	unsigned zavrsio;
	unsigned startovana;
	unsigned neograniceno;

	//BROJACI, FLAGOVI I LISTE
	unsigned stackSize;
	int brojac;
	Thread* myThread;
	List* waitList;
	ID id;
	int kvant;
	unsigned waitTime;
	int returnValue;

	PCB(StackSize stackSize, Time timeSlice,Thread* myThread);

	static void run();	//ovo je metoda wrapper

	static void initDefault();

	static int createProcess(PCB *newPCB);

	static void exitThread();
};


#endif
