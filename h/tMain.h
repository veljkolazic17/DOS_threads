/*
 * tMain.h
 *
 *  Created on: Jul 1, 2021
 *      Author: OS1
 */

#ifndef H_TMAIN_H_
#define H_TMAIN_H_
#include "../h/thread.h"

class tMain:public Thread{
private:
	int args;
	char** argv;
	int retback;
	static unsigned started;

public:
	tMain(int args,char** argv);
	void run();
	int getRetback() const;
	Thread* clone() const;
	~tMain();

	static int execute(int args,char**argv);
	static void restoreIVT();

};

#endif /* H_TMAIN_H_ */
