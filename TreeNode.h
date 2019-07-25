#pragma once

#include <stdlib.h>   // for size_t only
#include <algorithm>  // for std::max() only
#include <utility>    // for std::move() only
#include <vector>
#include <assert.h>

template<typename T>
class TreeNode
{
public:
  enum class Rotation {
    eRight,
    eLeft,
    eLeftRight,
    eRightLeft
  };

  TreeNode(T value)
    : value(std::move(value)), pParent(nullptr), pLeft(nullptr), pRight(nullptr),
      nLevel(1)
  {}

  // All this operations return pointer to new root of tree (or this, of root node
  // was nat changed)
  TreeNode<T>* insert(T value) { return insert(new TreeNode<T>(std::move(value))); }
  TreeNode<T>* remove(T const& value);
  TreeNode<T>* rebalance();

  bool     isBalanced() const { return abs(balance()) < 2; }
  bool     isLeaf()     const { return !pLeft && !pRight; }
  size_t   level()      const { return nLevel; }
  T const& data()       const { return value; }

  TreeNode<T>* find(T const& value) const;

  void mixedTraversal(std::vector<T>& out) const;

private:
  void updateLevel();
  TreeNode<T> *insert(TreeNode<T>* pNode);

  void onChildChanged(TreeNode<T>* pOldChild, TreeNode<T>* pNewChild);

  // Removing this element from subtree and returns pointer to the new subtree root
  // Release this element, if it is required ("delete this;" wil be called)
  TreeNode<T>* eraseSelf();

  T const& findMinimalValue() const;

  // if balance > 0, than right subtree is bigger than left subtree
  // if balance < 0, than left subtree is bigger than right subtree
  ssize_t balance() const;

  TreeNode<T>* rotateRight();
  TreeNode<T>* rotateLeft();
  TreeNode<T>* rotateLeftRight();
  TreeNode<T>* rotateRightLeft();

private:
  T            value;
  TreeNode<T>* pParent;
  TreeNode<T>* pLeft;
  TreeNode<T>* pRight;
  size_t       nLevel;
};

template<typename T>
TreeNode<T>* TreeNode<T>::insert(TreeNode<T>* pNode)
{
  TreeNode<T>*& pChild = (pNode->value <= value) ? pLeft : pRight;
  if (pChild) {
    pChild = pChild->insert(pNode);
  } else {
    pChild = pNode;
    pChild->pParent = this;
  }
  return rebalance();
}

template<typename T>
void TreeNode<T>::onChildChanged(TreeNode<T> *pOldChild, TreeNode<T> *pNewChild)
{
  if (pOldChild == pLeft)
    pLeft = pNewChild;
  else if (pOldChild == pRight)
    pRight = pNewChild;
}

template<typename T>
TreeNode<T>* TreeNode<T>::remove(T const& value)
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
TreeNode<T>* TreeNode<T>::rebalance()
{
  TreeNode<T>* pNewRoot = this;
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
TreeNode<T>* TreeNode<T>::find(T const& value) const
{
  if (this->value == value)
    return this;
  TreeNode<T>*& pChild = (value <= this->value) ? pLeft : pRight;
  return pChild ? pChild->find(value) : nullptr;
}

template<typename T>
void TreeNode<T>::mixedTraversal(std::vector<T>& out) const
{
  if (pLeft)
    pLeft->mixedTraversal(out);
  out.push_back(value);
  if (pRight)
    pRight->mixedTraversal(out);
}

template<typename T>
void TreeNode<T>::updateLevel()
{
  size_t nLeftLevel  = pLeft  ? pLeft->nLevel  : 0;
  size_t nRightLevel = pRight ? pRight->nLevel : 0;
  nLevel = std::max(nLeftLevel, nRightLevel) + 1;
}

template<typename T>
TreeNode<T>* TreeNode<T>::eraseSelf()
{
  TreeNode<T>* pNewRoot = nullptr;
  // if *this is a leaf, this function does nothing
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
T const& TreeNode<T>::findMinimalValue() const
{
  TreeNode<T> const* pNode = this;
  while (pNode->pLeft)
    pNode = pNode->pLeft;
  return pNode->value;
}

template<typename T>
ssize_t TreeNode<T>::balance() const
{
  size_t nLeftLevel  = pLeft  ? pLeft->nLevel  : 0;
  size_t nRightLevel = pRight ? pRight->nLevel : 0;
  return nRightLevel - nLeftLevel;
}

template<typename T>
TreeNode<T>* TreeNode<T>::rotateRight()
{
  assert(pLeft);
  if (!pLeft)
    return this;
  TreeNode<T>* pNewRoot = pLeft;
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
TreeNode<T>* TreeNode<T>::rotateLeft()
{
  assert(pRight);
  if (!pRight)
    return this;
  TreeNode<T>* pNewRoot = pRight;
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
TreeNode<T>* TreeNode<T>::rotateLeftRight()
{
  if (!pLeft)
    return this;
  pLeft = pLeft->rotateLeft();
  return rotateRight();
}

template<typename T>
TreeNode<T>* TreeNode<T>::rotateRightLeft()
{
  if (!pRight)
    return this;
  pRight = pRight->rotateRight();
  return rotateLeft();
}
