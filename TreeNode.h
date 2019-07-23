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
    : value(std::move(value)), pLeft(nullptr), pRight(nullptr), nLevel(1)
  {}

  void insert(T value) { insert(new TreeNode<T>(std::move(value))); }
  bool remove(T const& value);
  bool rebalance();

  bool     isBalanced() const { return abs(balance()) < 2; }
  bool     isLeaf()     const { return !pLeft && !pRight; }
  size_t   level()      const { return nLevel; }
  T const& data()       const { return value; }

  TreeNode<T>* find(T const& value) const;

  void mixedTraversal(std::vector<T>& out) const;

private:
  void updateLevel();
  void insert(TreeNode<T>* pNode);

  void swapWithOther(TreeNode<T>* pOther);
  void eraseSelf();
  void removeLeft();
  void removeRight();

  T const& findMinimalValue() const;

  // if balance > 0, than right subtree is bigger than left subtree
  // if balance < 0, than left subtree is bigger than right subtree
  ssize_t balance() const;

  bool rotateRight();
  bool rotateLeft();
  bool rotateLeftRight();
  bool rotateRightLeft();

private:
  T         value;
  TreeNode* pLeft;
  TreeNode* pRight;
  size_t    nLevel;
};

template<typename T>
void TreeNode<T>::insert(TreeNode<T>* pNode)
{
  TreeNode<T>*& pChild = (pNode->value <= value) ? pLeft : pRight;
  if (pChild) {
    pChild->insert(pNode);
  } else {
    pChild = pNode;
  }
  rebalance();
  updateLevel();
}

template<typename T>
bool TreeNode<T>::remove(T const& value)
{
  if (this->value == value) {
    assert(!isLeaf());
    eraseSelf();
    rebalance();
    updateLevel();
    return true;
  }

  bool          lSuccess = false;
  TreeNode<T>*& pChild   = (value <= this->value) ? pLeft : pRight;
  if (!pChild)
    return false;

  if (pChild->value != value) {
    lSuccess = pChild->remove(value);
  } else {
    // pChild contains exact value
    lSuccess = true;
    if (!pChild->isLeaf()) {
      pChild->eraseSelf();
    } else {
      delete pChild;
      pChild = nullptr;
    }
  }
  rebalance();
  updateLevel();
  return lSuccess;
}

template<typename T>
bool TreeNode<T>::rebalance()
{
  switch (balance()) {
    case -2: {
      // Left subtree is bigger then right subtree
      assert(pLeft);
      if (pLeft->balance() <= 0) {
        return rotateRight();
      } else {
        return rotateLeftRight();
      }
    }
    case 2: {
      // Right subtree is bigger than left subtree
      assert(pRight);
      if (pRight->balance() <= 0) {
        return rotateRightLeft();
      } else {
        return rotateLeft();
      }
    }
    default:
      // Tree is balanced already
      return true;
  }
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
void TreeNode<T>::swapWithOther(TreeNode<T> *pOther)
{  
  // Exchangin fields
  std::swap(value,  pOther->value);
  std::swap(nLevel, pOther->nLevel);
  std::swap(pLeft,  pOther->pLeft);
  std::swap(pRight, pOther->pRight);

  // One of this condition will be true, if one of node was a child of other node
  if (pLeft == this)
    pLeft = pOther;
  if (pRight == this)
    pRight = pOther;
  if (pOther->pLeft == pOther)
    pOther->pLeft = this;
  if (pOther->pRight == pOther)
    pOther->pRight = this;
}

template<typename T>
void TreeNode<T>::eraseSelf()
{
  // if *this is a leaf, this function does nothing
  if (pLeft && pRight) {
    value = pRight->findMinimalValue();
    if (!pRight->isLeaf()) {
      pRight->remove(value);
    } else {
      assert(pRight->value == value);
      delete pRight;
      pRight = nullptr;
    }
    return;
  }

  TreeNode<T>* pNodeToBeDeleted = nullptr;
  if (pLeft) {
    pNodeToBeDeleted = pLeft;
    this->swapWithOther(pLeft);
  } else if (pRight) {
    pNodeToBeDeleted = pRight;
    this->swapWithOther(pRight);
  }
  delete pNodeToBeDeleted;
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
bool TreeNode<T>::rotateRight()
{
  assert(pLeft);
  if (!pLeft)
    return false;
  TreeNode<T>* pLeftChild = pLeft;
  pLeft = pLeftChild->pRight;
  pLeftChild->pRight = this;
  pLeftChild->updateLevel();
  updateLevel();
  // This call makes "this" node the root of the tree again
  swapWithOther(pLeftChild);
  return true;
}

template<typename T>
bool TreeNode<T>::rotateLeft()
{
  assert(pRight);
  if (!pRight)
    return false;
  TreeNode<T>* pRightChild = pRight;
  pRight = pRightChild->pLeft;
  pRightChild->pLeft = this;
  pRightChild->updateLevel();
  updateLevel();
  // This call makes "this" node the root of the tree again
  swapWithOther(pRightChild);
  return true;
}

template<typename T>
bool TreeNode<T>::rotateLeftRight()
{
  return pLeft && pLeft->rotateLeft() && rotateRight();
}

template<typename T>
bool TreeNode<T>::rotateRightLeft()
{
  return pRight && pRight->rotateRight() && rotateLeft();
}
