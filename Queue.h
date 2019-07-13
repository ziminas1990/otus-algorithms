#pragma once

#include "Node.h"
#include <stdexcept>

template<typename T>
class Queue
{
public:

  bool empty() const { return pHead == nullptr; }

  void push(T item)
  {
    Node<T>* pNewTail = new Node<T>(std::move(item));
    if (pTail)
      pTail->setNext(pNewTail);
    pTail = pNewTail;
    if (!pHead)
      pHead = pTail;
  }

  T pop()
  {
    if (!pHead) {
      throw new std::out_of_range("list is empty");
    }
    T item = std::move(pHead->getItem());
    Node<T>* pNewHead = pHead->getNext();
    if (pHead == pTail)
      pTail = nullptr;
    delete pHead;
    pHead = pNewHead;
    return item;
  }

private:
  Node<T>* pHead = nullptr;
  Node<T>* pTail = nullptr;
};
