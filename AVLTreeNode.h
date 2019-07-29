#pragma once

#include <stdlib.h>   // for size_t only
#include <algorithm>  // for std::max() only
#include <utility>    // for std::move() only
#include <vector>
#include <assert.h>

template<typename T>
class AVLTreeNode
{
public:
  AVLTreeNode(T value)
    : value(std::move(value)), pParent(nullptr), pLeft(nullptr), pRight(nullptr),
      nLevel(1)
  {}

  // All this operations return pointer to new root of tree (or this, of root node
  // was nat changed)
  AVLTreeNode<T>* insert(T value) { return insert(new AVLTreeNode<T>(std::move(value))); }
  AVLTreeNode<T>* remove(T const& value);
  AVLTreeNode<T>* find(T const& value);

  bool     isBalanced() const { return abs(balance()) < 2; }
  bool     isLeaf()     const { return !pLeft && !pRight; }
  size_t   level()      const { return nLevel; }
  T const& data()       const { return value; }

  void mixedTraversal(std::vector<T>& out) const;

private:
  void updateLevel();
  AVLTreeNode<T> *insert(AVLTreeNode<T>* pNode);
  AVLTreeNode<T>* rebalance();

  void onChildChanged(AVLTreeNode<T>* pOldChild, AVLTreeNode<T>* pNewChild);

  // Removing this element from subtree and returns pointer to the new subtree root
  // Release this element, if it is required ("delete this;" wil be called)
  AVLTreeNode<T>* eraseSelf();

  T const& findMinimalValue() const;

  // if balance > 0, than right subtree is bigger than left subtree
  // if balance < 0, than left subtree is bigger than right subtree
  ssize_t balance() const;

  AVLTreeNode<T>* rotateRight();
  AVLTreeNode<T>* rotateLeft();
  AVLTreeNode<T>* rotateLeftRight();
  AVLTreeNode<T>* rotateRightLeft();

private:
  T               value;
  AVLTreeNode<T>* pParent;
  AVLTreeNode<T>* pLeft;
  AVLTreeNode<T>* pRight;
  size_t          nLevel;
};

template<typename T>
AVLTreeNode<T>* AVLTreeNode<T>::insert(AVLTreeNode<T>* pNode)
{
  AVLTreeNode<T>*& pChild = (pNode->value <= value) ? pLeft : pRight;
  if (pChild) {
    pChild = pChild->insert(pNode);
  } else {
    pChild = pNode;
    pChild->pParent = this;
  }
  return rebalance();
}

template<typename T>
void AVLTreeNode<T>::onChildChanged(AVLTreeNode<T> *pOldChild, AVLTreeNode<T> *pNewChild)
{
  if (pOldChild == pLeft)
    pLeft = pNewChild;
  else if (pOldChild == pRight)
    pRight = pNewChild;
}

template<typename T>
AVLTreeNode<T>* AVLTreeNode<T>::remove(T const& value)
{
  if (this->value == value) {
    return eraseSelf();
  } else if (value <= this->value && pLeft) {
    pLeft = pLeft->remove(value);
  } else if (pRight) {
    pRight = pRight->remove(value);
  }
  return rebalance();
}

template<typename T>
AVLTreeNode<T>* AVLTreeNode<T>::find(T const& value)
{
  if (this->value == value)
    return this;
  AVLTreeNode<T>*& pChild = (value <= this->value) ? pLeft : pRight;
  return pChild ? pChild->find(value) : nullptr;
}

template<typename T>
void AVLTreeNode<T>::mixedTraversal(std::vector<T>& out) const
{
  if (pLeft)
    pLeft->mixedTraversal(out);
  out.push_back(value);
  if (pRight)
    pRight->mixedTraversal(out);
}

template<typename T>
AVLTreeNode<T>* AVLTreeNode<T>::rebalance()
{
  AVLTreeNode<T>* pNewRoot = this;
  switch (balance()) {
    case -2: {
      // Left subtree is bigger then right subtree
      assert(pLeft);
      pNewRoot = (pLeft->balance() <= 0) ? rotateRight() : rotateLeftRight();
      break;
    }
    case 2: {
      // Right subtree is bigger than left subtree
      assert(pRight);
      pNewRoot = (pRight->balance() <= 0) ? rotateRightLeft() : rotateLeft();
      break;
    }
  }
  pNewRoot->updateLevel();
  return pNewRoot;
}

template<typename T>
void AVLTreeNode<T>::updateLevel()
{
  size_t nLeftLevel  = pLeft  ? pLeft->nLevel  : 0;
  size_t nRightLevel = pRight ? pRight->nLevel : 0;
  nLevel = std::max(nLeftLevel, nRightLevel) + 1;
}

template<typename T>
AVLTreeNode<T>* AVLTreeNode<T>::eraseSelf()
{
  AVLTreeNode<T>* pNewRoot = nullptr;
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
  return pNewRoot ? pNewRoot->rebalance() : nullptr;
}

template<typename T>
T const& AVLTreeNode<T>::findMinimalValue() const
{
  AVLTreeNode<T> const* pNode = this;
  while (pNode->pLeft)
    pNode = pNode->pLeft;
  return pNode->value;
}

template<typename T>
ssize_t AVLTreeNode<T>::balance() const
{
  size_t nLeftLevel  = pLeft  ? pLeft->nLevel  : 0;
  size_t nRightLevel = pRight ? pRight->nLevel : 0;
  return nRightLevel - nLeftLevel;
}

template<typename T>
AVLTreeNode<T>* AVLTreeNode<T>::rotateRight()
{
  assert(pLeft);
  if (!pLeft)
    return this;
  AVLTreeNode<T>* pNewRoot = pLeft;
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
AVLTreeNode<T>* AVLTreeNode<T>::rotateLeft()
{
  assert(pRight);
  if (!pRight)
    return this;
  AVLTreeNode<T>* pNewRoot = pRight;
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
AVLTreeNode<T>* AVLTreeNode<T>::rotateLeftRight()
{
  if (!pLeft)
    return this;
  pLeft = pLeft->rotateLeft();
  return rotateRight();
}

template<typename T>
AVLTreeNode<T>* AVLTreeNode<T>::rotateRightLeft()
{
  if (!pRight)
    return this;
  pRight = pRight->rotateRight();
  return rotateLeft();
}
