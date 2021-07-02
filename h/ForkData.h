/*
 * ForkData.h
 *
 *  Created on: Jul 1, 2021
 *      Author: OS1
 */

#ifndef H_FORKDATA_H_
#define H_FORKDATA_H_

class Thread;
class PCB;

class ForkData {
public:
	static Thread* deteThread;
	static PCB* detePCB;
	static void interrupt snap();

	static unsigned ss_;
	static unsigned sp_;
	static unsigned bp_;
};

#endif /* H_FORKDATA_H_ */
