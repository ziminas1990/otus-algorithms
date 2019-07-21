#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <assert.h>

#include "Stopwatch.h"

template<typename T>
class TreeNode
{
public:
  TreeNode(T&& value, TreeNode* pParent = nullptr)
    : value(std::move(value)), pLeft(nullptr), pRight(nullptr), nLevel(1)
  {}

  void insert(T value) { insert(new TreeNode<T>(value)); }
  bool remove(T const& value);
  TreeNode<T>* find(T const& value) const;

private:
  void updateLevel();
  void insert(TreeNode<T>* pNode);

  void removeLeft();
  void removeRight();

  TreeNode<T> *search(T const& value, TreeNode<T> *&pParent) const;

private:
  T         value;
  TreeNode* pParent;
  TreeNode* pLeft;
  TreeNode* pRight;
  size_t    nLevel;
};

template<typename T>
bool TreeNode<T>::remove(T const& value)
{
  TreeNode* pParent       = this;
  TreeNode* pNodeToRemove = search(value, pParent);
  if (!pNodeToRemove)
    return false;

  assert(pNodeToRemove == pParent->pLeft || pNodeToRemove == pParent->pRight);
  if (pNodeToRemove == pParent->pLeft) {
    pParent->removeLeft();
  } else {
    pParent->removeRight();
  }
  return true;
}

template<typename T>
TreeNode<T>* TreeNode<T>::find(T const& value) const
{
  TreeNode* pParent;
  return search(value, pParent);
}

template<typename T>
TreeNode<T>* TreeNode<T>::search(T const& value, TreeNode<T>*& pParent) const
{
  if (this->value == value) {
    return this;
  }
  pParent = this;
  if (value < this->value) {
    return pLeft ? pLeft->search(value) : nullptr;
  } else {
    return pRight ? pRight->search(value) : nullptr;
  }
}

template<typename T>
void TreeNode<T>::updateLevel()
{
  size_t nLeftLevel  = pLeft  ? pLeft->nLevel  : 0;
  size_t nRightLevel = pRight ? pRight->nLevel : 0;
  nLevel = std::max(nLeftLevel, nRightLevel) + 1;
}

template<typename T>
void TreeNode<T>::insert(TreeNode<T>* pNode)
{
  if (pNode->value <= value) {
    if (pLeft) {
      pLeft->insert(pNode);
    } else {
      pLeft = pNode;
      pLeft->pParent = this;
    }
  } else {
    if (pRight) {
      pRight->insert(pNode);
    } else {
      pRight = pNode;
      pRight->pParent = this;
    }
  }
  updateLevel();
}

template<typename T>
void TreeNode<T>::removeLeft()
{
  if (!pLeft)
    return;
  TreeNode* pNodeToRemove = pLeft;
  if (pNodeToRemove->pLeft) {
    pLeft = pNodeToRemove->pLeft;
    if (pNodeToRemove->pRight)
      pLeft->insert(pNodeToRemove->pRight);
  } else {
    pLeft = pNodeToRemove->pRight;
  }
  delete pNodeToRemove;
}

template<typename T>
void TreeNode<T>::removeRight()
{
  if (!pRight)
    return;
  TreeNode* pNodeToRemove = pRight;
  if (pNodeToRemove->pRight) {
    pRight = pNodeToRemove->pRight;
    if (pNodeToRemove->pLeft)
      pRight->insert(pNodeToRemove->pLeft);
  } else {
    pRight = pNodeToRemove->pLeft;
  }
  delete pNodeToRemove;
}


int main(int argc, char* argv[])
{
  std::srand(time(0));
  return 0;
}
