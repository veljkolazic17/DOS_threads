#include "../h/iterator.h"

void* Iterator::iterateNext(){
    if(current != NULL){
      void* dtemp = current->data;
      current = current->next;
      return dtemp;
    }
    return NULL;
  }
  void* Iterator::iterateBack(){
    if(current != NULL){
      void* dtemp = current->data;
      current = current->last;
      return dtemp;
    }
    return NULL;
  }
  void Iterator::iteratorReset(){
    current = first;
  }
  void* Iterator::currentData(){
    if(current == NULL)
      return NULL;
    return current->data;
  }