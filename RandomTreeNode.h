#pragma once

#include <stdlib.h>   // for size_t only
#include <algorithm>  // for std::max() only
#include <utility>    // for std::move() only
#include <vector>
#include <assert.h>

template<typename T>
class RandomTreeNode
{
public:
  RandomTreeNode(T value)
    : value(std::move(value)), pParent(nullptr), pLeft(nullptr), pRight(nullptr),
      nLevel(1)
  {}

  // All this operations return pointer to new root of tree (or this, of root node
  // was nat changed)
  virtual RandomTreeNode<T>* insert(T value) {
    return insert(new RandomTreeNode<T>(std::move(value)));
  }
  virtual RandomTreeNode<T>* remove(T const& value);

  bool     isLeaf()     const { return !pLeft && !pRight; }
  size_t   level()      const { return nLevel; }
  T const& data()       const { return value; }

  void mixedTraversal(std::vector<T>& out) const;

private:
  void updateLevel();
  RandomTreeNode<T>* insert(RandomTreeNode<T>* pNode);
  RandomTreeNode<T>* insertAsRoot(RandomTreeNode<T>* pNode);

  RandomTreeNode<T>* liftToRoot(T const& value, bool& lSuccess);

  T findMinimalValue() const;

  void onChildChanged(RandomTreeNode<T>* pOldChild, RandomTreeNode<T>* pNewChild);

  // Removing this element from subtree and returns pointer to the new subtree root
  // Release this element, if it is required ("delete this;" wil be called)
  RandomTreeNode<T>* eraseSelf();

  RandomTreeNode<T>* rotateRight();
  RandomTreeNode<T>* rotateLeft();
  RandomTreeNode<T>* rotateLeftRight();
  RandomTreeNode<T>* rotateRightLeft();

private:
  T            value;
  RandomTreeNode<T>* pParent;
  RandomTreeNode<T>* pLeft;
  RandomTreeNode<T>* pRight;
  size_t       nLevel;
};

template<typename T>
RandomTreeNode<T>* RandomTreeNode<T>::insert(RandomTreeNode<T>* pNode)
{
  if (std::rand() % nLevel == 0) {
    return insertAsRoot(pNode);
  }
  RandomTreeNode<T>*& pChild = (pNode->value <= value) ? pLeft : pRight;
  pChild = pChild ? pChild->insert(pNode) : pNode;
  updateLevel();
  return this;
}

template<typename T>
RandomTreeNode<T>* RandomTreeNode<T>::insertAsRoot(RandomTreeNode<T>* pNode)
{
  if (pNode->value <= value) {
    pLeft = pLeft ? pLeft->insertAsRoot(pNode) : pNode;
    return rotateRight();
  } else {
    pRight = pRight ? pRight->insertAsRoot(pNode) : pNode;
    return rotateLeft();
  }
}

template<typename T>
RandomTreeNode<T>* RandomTreeNode<T>::liftToRoot(T const& value, bool& lSuccess)
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
T RandomTreeNode<T>::findMinimalValue() const
{
  RandomTreeNode<T> const* pNode = this;
  while (pNode->pLeft)
    pNode = pNode->pLeft;
  return pNode->value;
}

template<typename T>
void RandomTreeNode<T>::onChildChanged(RandomTreeNode<T> *pOldChild,
                                       RandomTreeNode<T> *pNewChild)
{
  if (pOldChild == pLeft)
    pLeft = pNewChild;
  else if (pOldChild == pRight)
    pRight = pNewChild;
}

template<typename T>
RandomTreeNode<T>* RandomTreeNode<T>::remove(T const& value)
{
  if (std::rand() % nLevel == 0) {
    bool lSuccess = false;
    RandomTreeNode<T>* pNewRoot = liftToRoot(value, lSuccess);
    // if lSuccess is true, that means that this node becomes a child of pNewRoot
    // if lSuccess is false, that means that node with value was not found and this node
    // is still a root of it's tree and pNewRoot == this
    return lSuccess ? pNewRoot->eraseSelf() : pNewRoot;
  }

  if (this->value == value) {
    return eraseSelf();
  }

  RandomTreeNode<T>*& pChild = (value < this->value) ? pLeft : pRight;
  if (pChild)
    pChild = pChild->remove(value);
  return this;
}

template<typename T>
void RandomTreeNode<T>::mixedTraversal(std::vector<T>& out) const
{
  if (pLeft)
    pLeft->mixedTraversal(out);
  out.push_back(value);
  if (pRight)
    pRight->mixedTraversal(out);
}

template<typename T>
void RandomTreeNode<T>::updateLevel()
{
  size_t nLeftLevel  = pLeft  ? pLeft->nLevel  : 0;
  size_t nRightLevel = pRight ? pRight->nLevel : 0;
  nLevel = std::max(nLeftLevel, nRightLevel) + 1;
}

template<typename T>
RandomTreeNode<T>* RandomTreeNode<T>::eraseSelf()
{
  RandomTreeNode<T>* pNewRoot = nullptr;
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
RandomTreeNode<T>* RandomTreeNode<T>::rotateRight()
{
  assert(pLeft);
  if (!pLeft)
    return this;
  RandomTreeNode<T>* pNewRoot = pLeft;
  if (pParent)
    pParent->onChildChanged(this, pNewRoot);

  pNewRoot->pParent = pParent;
  pParent           = pNewRoot;
  pLeft             = pNewRoot->pRight;
  pNewRoot->pRight  = this;

  updateLevel();
  pNewRoot->updateLevel();
  return pNewRoot;
}

template<typename T>
RandomTreeNode<T>* RandomTreeNode<T>::rotateLeft()
{
  assert(pRight);
  if (!pRight)
    return this;
  RandomTreeNode<T>* pNewRoot = pRight;
  if (pParent)
    pParent->onChildChanged(this, pNewRoot);

  pNewRoot->pParent = pParent;
  pParent           = pNewRoot;
  pRight            = pNewRoot->pLeft;
  pNewRoot->pLeft   = this;

  updateLevel();
  pNewRoot->updateLevel();
  return pNewRoot;
}

template<typename T>
RandomTreeNode<T>* RandomTreeNode<T>::rotateLeftRight()
{
  if (!pLeft)
    return this;
  pLeft = pLeft->rotateLeft();
  return rotateRight();
}

template<typename T>
RandomTreeNode<T>* RandomTreeNode<T>::rotateRightLeft()
{
  if (!pRight)
    return this;
  pRight = pRight->rotateRight();
  return rotateLeft();
}
