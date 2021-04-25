#ifndef _LLIST_H_
#define _LLIST_H_
#include<iostream.h>//javlja gresku zbog iostreama i dosa
#include<stdlib.h>

extern volatile unsigned int lockFlag;//lockovanje zone bez ukidanja prekida

class List{
public:
  struct Node{
    void* data;
    Node* next;
    Node* last;

    Node(void* arg_data){
      data = arg_data;
    }
  };

  Node* first;
  Node* last;
  Node* current;
  unsigned int length;



  List(){
    length = 0;
    first = NULL;
    last = NULL;
    current = NULL;
  }

  void putNext(void* data) volatile{
      if(length == 0){
        first = new Node(data);
        current = first;
        last = first;
        first->last = NULL;
        first->next = NULL;
      }
      else{
        Node* temp = new Node(data);
        temp->last = last;
        last->next = temp;
        last = temp;
        temp->next = NULL;
      }
      length++;
  }
  void putBack(void* data) volatile{
      if(length == 0){
        first = new Node(data);
        current = first;
        last = first;
        first->last = NULL;
        first->next = NULL;
      }
      else{
        Node* temp = new Node(data);
        temp->next = first;
        first->last = temp;
        if(current == first){
          current = temp;
        }
        first = temp;
      }
      length++;
  }
  void* iterateNext(){
    if(current != NULL){
      void* dtemp = current->data;
      current = current->next;
      return dtemp;
    }
    return NULL;
  }
  void* iterateBack(){
    if(current != NULL){
      void* dtemp = current->data;
      current = current->last;
      return dtemp;
    }
    return NULL;
  }
  void iteratorReset(){
    current = first;
  }
  void* currentData(){
    if(current == NULL)
      return NULL;
    return current->data;
  }
  unsigned removeAtPCB(ID id) volatile;
};
#endif
