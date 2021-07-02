/*
 * ForkData.cpp
 *
 *  Created on: Jul 1, 2021
 *      Author: OS1
 */

#include "../h/ForkData.h"
#include "../h/PCB.h"
#include<string.h>
PCB* ForkData::detePCB = 0;
Thread* ForkData::deteThread = 0;
unsigned ForkData::ss_;
unsigned ForkData::sp_;
unsigned ForkData::bp_;

unsigned sp_temp,bp_temp,ss_temp;

void interrupt ForkData::snap(){
	memcpy(ForkData::detePCB->stack,PCB::running->stack,PCB::running->stackSize);
	asm{
		mov ss_temp ,ss
		mov sp_temp ,sp
		mov bp_temp ,bp
	}
	ForkData::ss_ = ss_temp;
	ForkData::sp_ = sp_temp;
	ForkData::bp_ = bp_temp;
}
