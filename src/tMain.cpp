/*
 * tMain.cpp
 *
 *  Created on: Jul 1, 2021
 *      Author: OS1
 */

#include "../h/tMain.h"
#include "../h/ttools.h"
#include "../h/IVTEntry.h"
#include "../h/Iterator.h"

unsigned tMain::started = 0;

int userMain(int args,char* argv[]);

void tMain::run(){
	this->retback = userMain(args,argv);
}

tMain::tMain(int args,char** argv){
	this->args = args;
	this->argv = argv;
	this->retback = -1;
}

tMain::~tMain(){
	this->waitToComplete();
}

Thread* tMain::clone() const{
	tMain* klon = new tMain(this->args,this->argv);
	klon->retback = this->retback;
	return klon;
}

int tMain::getRetback()const{
	return this->retback;
}

int tMain::execute(int args,char**argv){
	if(!started){
		initDefaultWrapper();
		inic();
		tMain tmain(args,argv);
		tmain.start();
		tmain.waitToComplete();
		int retback = tmain.getRetback();
		restore();
		tMain::started = 1;
		return retback;
	}
	//TODO pitanje da li linija ispod treba da stoji
	tMain::restoreIVT();
	return -1;
}

void tMain::restoreIVT(){
	Iterator* iterator = new Iterator((List*)Shared::IVTEntries);
	IVTEntry* entry = NULL;
	while((entry = (IVTEntry*)iterator->iterateNext()) != NULL){

		delete entry;
	}
	Shared::IVTEntries->purge();
	delete iterator;
}
