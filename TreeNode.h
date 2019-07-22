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
  TreeNode(T&& value)
    : value(std::move(value)), pLeft(nullptr), pRight(nullptr), nLevel(1)
  {}

  void     insert(T value) { insert(new TreeNode<T>(std::move(value))); }
  bool     remove(T const& value);
  bool     isLeaf() const { return !pLeft && !pRight; }
  size_t   level()  const { return nLevel; }
  T const& data()   const { return value; }

  TreeNode<T>* find(T const& value) const;

  void mixedTraversal(std::vector<T>& out) const;

private:
  void updateLevel();
  void insert(TreeNode<T>* pNode);

  void replaceSelfWith(TreeNode<T>* pOther);
  void eraseSelf();
  void removeLeft();
  void removeRight();

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
  updateLevel();
}

template<typename T>
bool TreeNode<T>::remove(T const& value)
{
  if (this->value == value) {
    assert(!isLeaf());
    eraseSelf();
    return true;
  }

  bool          lSuccess = false;
  TreeNode<T>*& pChild   = (value <= this->value) ? pLeft : pRight;

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
  updateLevel();
  return lSuccess;
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
void TreeNode<T>::replaceSelfWith(TreeNode<T> *pOther)
{
  value  = std::move(pOther->value);
  pLeft  = pOther->pLeft;
  pRight = pOther->pRight;
  pOther->pLeft  = nullptr;
  pOther->pRight = nullptr;
  delete pOther;
}

template<typename T>
void TreeNode<T>::eraseSelf()
{
  // if *this is a leaf, this function does nothing
  if (pLeft) {
    if (pRight)
      pLeft->insert(pRight);
    this->replaceSelfWith(pLeft);
  } else if (pRight) {
    this->replaceSelfWith(pRight);
  }
}
