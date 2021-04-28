#include "../h/thread.h"
#include "../h/PCB.h"
#include "../h/shared.h"

ID Thread::getId(){
    return this->myPCB->id;
}

Thread::Thread(StackSize stackSize, Time timeSlice){
    lock // proveri da li treba lockFlag
    this->myPCB = new PCB(stackSize,timeSlice,this);
    unlock
}

Thread::~Thread(){
    Shared::lista->removeAtPCB(this->getId());
}

void Thread::waitToComplete(){
    Shared::lockFlag = 0;
    if (!this->myPCB->zavrsio){
        this->myPCB->waitList->putNext((PCB*)PCB::running);
        PCB::running->blokirana = 1;
        //cout<<"Blokirana: " << (void*)PCB::running << "\n";
    }
    Shared::lockFlag = 1;
    dispatch();
}

void Thread::start(){
    if(this->myPCB->startovana == 1)
        return;
    Shared::lockFlag = 0; //provei da li treba lock ili moze ovako
    PCB::createProcess(this->myPCB);
    this->myPCB->startovana = 1;
    Scheduler::put(this->myPCB);
    Shared::lockFlag = 1;
}

class T1:public Thread{
public:
    T1():Thread(1100,1){
    }
    void run(){
        for(int i = 0;i<10;i++){
            Shared::lockFlag = 0;
            cout<<"Thread --- 1 ---" << i << "\n";
            Shared::lockFlag = 1;
            for(int k = 0;k<3000;k++){
                for(int j = 0;j<300;j++){

                }
            }
        }
        Shared::lockFlag = 0;
            cout<<"Thread 1 gotov\n";
        Shared::lockFlag = 1;
    }
};

class T2:public Thread{
public:
    T2():Thread(500,1){
      //cout<<"Napravio sam Thread2\n";
    }
    void run(){
        for(int i = 0;i<100;i++){
            Shared::lockFlag = 0;
            cout<<"Thread --- 2 ---" << i << "\n";
            Shared::lockFlag = 1;
            for(int k = 0;k<3000;k++){
                for(int j = 0;j<3000;j++){

                }
            }
        }  
        Shared::lockFlag = 0;
            cout<<"Thread 2 gotov\n";
        Shared::lockFlag = 1;
    }
};
class T3:public Thread{
public:
    T3():Thread(1200,2){
      //cout<<"Napravio sam Thread3\n";
    }
    void run(){
        for(int i = 0;i<10;i++){
            Shared::lockFlag = 0;
            cout<<"Thread --- 3 ---\n";
            Shared::lockFlag = 1;
            for(int k = 0;k<3000;k++){
                for(int j = 0;j<300;j++){

                }
            }
        }
        Shared::lockFlag = 0;
            cout<<"Thread 3 gotov\n";
        Shared::lockFlag = 1;
    }
};


int main(){
    inic();
    initDefaultWrapper();
    T1* t1 = new T1();
    T2* t2 = new T2();
    T3* t3 = new T3();
    
    t3->start();
    t2->start();
    t1->start();
    
    
    for(int k = 0;k<10;k++){
            //if (k == 4) t2->waitToComplete();
            //if (k == 7) t3->waitToComplete();
            cout << "Main: " << k << endl;
            delay(1000);
            
    }

    
    //t2->waitToComplete();
    t3->waitToComplete();
    t1->waitToComplete();
    //delay(100000);
    cout<<"Zavrsio sam\n";
    
    restore();

    return 0;
}