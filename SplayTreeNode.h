#pragma once

#include <stdlib.h>   // for size_t only
#include <algorithm>  // for std::max() only
#include <utility>    // for std::move() only
#include <vector>
#include <assert.h>

template<typename T>
class SplayTreeNode
{
public:
  SplayTreeNode(T value)
    : value(std::move(value)), pParent(nullptr), pLeft(nullptr), pRight(nullptr)
  {}

  // All this operations return pointer to new root of tree (or this, of root node
  // was nat changed)
  SplayTreeNode<T>* insert(T value) {
    return insert(new SplayTreeNode<T>(std::move(value)));
  }
  SplayTreeNode<T>* remove(T const& value);

  SplayTreeNode<T>* find(T const& value);

  bool     isLeaf() const { return !pLeft && !pRight; }
  T const& data()   const { return value; }

  void mixedTraversal(std::vector<T>& out) const;

private:
  SplayTreeNode<T>* insert(SplayTreeNode<T>* pNode);

  SplayTreeNode<T>* liftToRoot(T const& value, bool& lSuccess);

  T findMinimalValue() const;

  void onChildChanged(SplayTreeNode<T>* pOldChild, SplayTreeNode<T>* pNewChild);

  // Removing this element from subtree and returns pointer to the new subtree root
  // Release this element, if it is required ("delete this;" wil be called)
  SplayTreeNode<T>* eraseSelf();

  SplayTreeNode<T>* rotateRight();
  SplayTreeNode<T>* rotateLeft();

private:
  T value;
  SplayTreeNode<T>* pParent;
  SplayTreeNode<T>* pLeft;
  SplayTreeNode<T>* pRight;
};

template<typename T>
SplayTreeNode<T>* SplayTreeNode<T>::insert(SplayTreeNode<T>* pNode)
{
  if (pNode->value <= value) {
    pLeft = pLeft ? pLeft->insert(pNode) : pNode;
    return rotateRight();
  } else {
    pRight = pRight ? pRight->insert(pNode) : pNode;
    return rotateLeft();
  }
}

template<typename T>
SplayTreeNode<T>* SplayTreeNode<T>::remove(T const& value)
{
  bool lSuccess;
  SplayTreeNode<T>* pNewRoot = liftToRoot(value, lSuccess);
  return lSuccess ? pNewRoot->eraseSelf() : this;
}

template<typename T>
SplayTreeNode<T>* SplayTreeNode<T>::find(T const& value)
{
  bool lFound = false;
  SplayTreeNode<T>* pNewRoot = liftToRoot(value, lFound);
  return lFound ? pNewRoot : nullptr;
}

template<typename T>
void SplayTreeNode<T>::mixedTraversal(std::vector<T>& out) const
{
  if (pLeft)
    pLeft->mixedTraversal(out);
  out.push_back(value);
  if (pRight)
    pRight->mixedTraversal(out);
}


template<typename T>
SplayTreeNode<T>* SplayTreeNode<T>::liftToRoot(T const& value, bool& lSuccess)
{
  if (value < this->value && pLeft) {
    pLeft = pLeft->liftToRoot(value, lSuccess);
    return lSuccess ? rotateRight() : this;
  } else if (value > this->value && pRight) {
    pRight = pRight->liftToRoot(value, lSuccess);
    return lSuccess ? rotateLeft() : this;
  }
  lSuccess = (this->value == value);
  return this;
}

template<typename T>
T SplayTreeNode<T>::findMinimalValue() const
{
  SplayTreeNode<T> const* pNode = this;
  while (pNode->pLeft)
    pNode = pNode->pLeft;
  return pNode->value;
}

template<typename T>
void SplayTreeNode<T>::onChildChanged(SplayTreeNode<T> *pOldChild,
                                       SplayTreeNode<T> *pNewChild)
{
  if (pOldChild == pLeft)
    pLeft = pNewChild;
  else if (pOldChild == pRight)
    pRight = pNewChild;
}

template<typename T>
SplayTreeNode<T>* SplayTreeNode<T>::eraseSelf()
{
  SplayTreeNode<T>* pNewRoot = nullptr;
  if (pLeft && pRight) {
    value  = pRight->findMinimalValue();
    pRight = pRight->remove(value);
    pNewRoot = this;
  } else {
    pNewRoot = pLeft ? pLeft : pRight;
    if (pNewRoot)
      pNewRoot->pParent = pParent;
    delete this;
  }
  return pNewRoot;
}

template<typename T>
SplayTreeNode<T>* SplayTreeNode<T>::rotateRight()
{
  assert(pLeft);
  if (!pLeft)
    return this;
  SplayTreeNode<T>* pNewRoot = pLeft;
  if (pParent)
    pParent->onChildChanged(this, pNewRoot);

  pNewRoot->pParent = pParent;
  pParent           = pNewRoot;
  pLeft             = pNewRoot->pRight;
  pNewRoot->pRight  = this;
  return pNewRoot;
}

template<typename T>
SplayTreeNode<T>* SplayTreeNode<T>::rotateLeft()
{
  assert(pRight);
  if (!pRight)
    return this;
  SplayTreeNode<T>* pNewRoot = pRight;
  if (pParent)
    pParent->onChildChanged(this, pNewRoot);

  pNewRoot->pParent = pParent;
  pParent           = pNewRoot;
  pRight            = pNewRoot->pLeft;
  pNewRoot->pLeft   = this;
  return pNewRoot;
}
