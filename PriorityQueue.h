#include "Node.h"
#include "Queue.h"

#include <stdint.h>

template<typename T>
class PriorityQueue
{
private:

  // just extends Queue with priority label
  template<typename ItemType>
  class LabledQueue : public Queue<ItemType>
  {
  public:
    LabledQueue(uint8_t nPriority) : nPriority(nPriority) {}

    uint8_t getPriority() const { return nPriority; }
    void    setPriority(uint8_t nPriority) { this->nPriority = nPriority; }
  private:
    uint8_t nPriority;
  };

public:

  PriorityQueue() : pHead(nullptr) {}

  // NOTE:
  // nPriority = 0    - the highest priority
  // nPriority = 0xFF - the lowest priority
  void enqueue(uint8_t nPriority, T item)
  {
    LabledQueue<T>& queue = getOrCreateQueue(nPriority);
    queue.push(std::move(item));
  }

  T dequeue()
  {
    if (!pHead) {
      throw new std::out_of_range("queue is empty");
    }

    LabledQueue<T>& queue = pHead->getItem();
    T item = queue.pop();
    if (queue.empty()) {
      Node<LabledQueue<T>>* pNewHead = pHead->getNext();
      delete pHead;
      pHead = pNewHead;
    }
    return item;
  }

private:
  LabledQueue<T>& getOrCreateQueue(uint8_t nPriority)
  {
    if (!pHead) {
      pHead = new Node<LabledQueue<T>>(nPriority);
      return pHead->getItem();
    }

    Node<LabledQueue<T>>* pNode = pHead;
    while(true) {
      uint8_t nNodePriority = pNode->getItem().getPriority();
      if (nNodePriority == nPriority) {
        return pNode->getItem();

      } else if (nNodePriority > nPriority) {
        // now: ... -> A -> ..., where A = pNode
        Node<LabledQueue<T>>* pNewNode = new Node<LabledQueue<T>>(nPriority);
        pNewNode->setNext(pNode->getNext());
        pNode->setNext(pNewNode);
        // now: ... -> A -> B -> ..., where pNode = A, pNewNode = B
        pNode->swapData(*pNewNode);
        // now: ... -> B -> A -> ..., where pNode = B, pNewNode = A
        return pNode->getItem();

      } else if (!pNode->getNext()) {
        Node<LabledQueue<T>>* pNewNode = new Node<LabledQueue<T>>(nPriority);
        pNode->setNext(pNewNode);
        return pNewNode->getItem();
      }
      pNode = pNode->getNext();
    }
  }

private:
  Node<LabledQueue<T>>* pHead;
};
