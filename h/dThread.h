#ifndef _DefaultThread_H_
#define _DefaultThread_H_
#include "thread.h"

#include<stdio.h>

class DefaultThread:public Thread{
public:
    DefaultThread():Thread(100,1){
    }
    void run(){
        while(1){}
    }
};
#endif
