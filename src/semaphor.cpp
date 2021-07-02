#include "../h/semaphor.h"
#include "../h/ksem.h"
#include "../h/ttools.h"

Semaphore::Semaphore(int init){
	lockf();
    this->myImpl = new KernelSem(init);
	unlockf();
}

int Semaphore::val() const{
    return this->myImpl->value;
}

Semaphore::~Semaphore(){
	lockf();
    delete this->myImpl;
	unlockf();
}

int Semaphore::wait(Time maxTime){
    return this->myImpl->wait(maxTime);
}

void Semaphore::signal(){
    this->myImpl->signal();
}
