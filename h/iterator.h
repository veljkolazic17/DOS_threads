#ifndef _ITERATOR_H_
#define _ITERATOR_H_
#include "llist.h"
#include<stdlib.h>

class Iterator{
public:
    List::Node* first;
    List::Node* last;
    List::Node* current;

    Iterator(List* lista){
        this->current = lista->first;
        this->first = lista->first;
        this->last = lista->last;
    }

    void* iterateNext();

    void* iterateBack();

    void iteratorReset();

    void* currentData();

    friend class List;
};

#endif