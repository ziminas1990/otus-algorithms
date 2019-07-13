#pragma once

#include <utility>

template<typename T>
class Node
{
public:
  Node(T item, Node<T>* pNext = nullptr)
    : item(std::move(item)), pNext(pNext)
  {}

  T&       getItem()       { return item; }
  T const& getItem() const { return item; }
  Node<T>* getNext() const { return pNext; }

  void setNext(Node<T>* pNext) { this->pNext = pNext; }

  // exchange data with other node (only data, NOT pNext!)
  void swapData(Node& other) { std::swap(item, other.item); }

private:
  T        item;
  Node<T>* pNext;
};
