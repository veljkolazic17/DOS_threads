#include "../h/kernelEv.h"
#include "../h/ttools.h"


Event::Event(IVTNo ivtNo){
    this->myImpl = new KernelEv(ivtNo);
}

void Event::wait(){
    this->myImpl->wait();
}

void Event::signal(){
    this->myImpl->signal();
}

Event::~Event(){
    locknest
    delete this->myImpl;
    unlocknest
}
