#include "../h/semaphor.h"
#include "../h/ksem.h"
#include "../h/ttools.h"

Semaphore::Semaphore(int init){
    this->myImpl = new KernelSem(init);
}

int Semaphore::val() const{
    return this->myImpl->value;
}

Semaphore::~Semaphore(){
    delete this->myImpl;
}

int Semaphore::wait(Time maxTime){
    return this->myImpl->wait(maxTime);
}

void Semaphore::signal(){
    this->myImpl->signal();
}
