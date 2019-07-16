#include <stdlib.h>
#include <stdint.h>
#include <algorithm>
#include <ctime>
#include <iostream>

// Handy struct for manipulating with array as with binary tree
// Almost zero overhead
struct BinaryTreeNode
{
  uint32_t* pRoot        = nullptr;
  size_t    nLength      = 0;
  size_t    nNodeIndex   = 0;

  size_t    nParentIndex = 0;
  size_t    nLeftIndex   = 0;
  size_t    nRightIndex  = 0;


  BinaryTreeNode() = default;

  BinaryTreeNode(uint32_t* pRoot, size_t nLength, size_t nNodeIndex = 0)
    : pRoot(pRoot), nLength(nLength), nNodeIndex(nNodeIndex),
      nParentIndex((nNodeIndex - 1) / 2),
      nLeftIndex(nNodeIndex * 2 + 1),
      nRightIndex(nLeftIndex + 1)
  {}

  bool hasLeft()   const { return nLeftIndex < nLength; }
  bool hasRight()  const { return nRightIndex < nLength; }
  bool hasParent() const { return nNodeIndex > 0; }

  uint32_t self()   const { return pRoot[nNodeIndex]; }
  uint32_t left()   const { return pRoot[nLeftIndex]; }
  uint32_t right()  const { return pRoot[nRightIndex]; }
  uint32_t parent() const { return pRoot[nParentIndex]; }

  BinaryTreeNode leftSubtree() {
    return hasLeft() ? BinaryTreeNode(pRoot, nLength, nLeftIndex) : BinaryTreeNode();
  }

  BinaryTreeNode rightSubtree() {
    return hasRight() ? BinaryTreeNode(pRoot, nLength, nRightIndex) : BinaryTreeNode();
  }

  void exchangeWithLeft() {
    if (hasLeft())
      std::swap(pRoot[nLeftIndex], pRoot[nNodeIndex]);
  }

  bool exchangeWithRight() {
    if (hasRight())
      std::swap(pRoot[nRightIndex], pRoot[nNodeIndex]);
  }
};

void drown(BinaryTreeNode pNode)
{
  if (!pNode.hasLeft())
    // pNode has no children
    return;
  if (!pNode.hasRight()) {
    // pNode has only left child
    if (pNode.self() < pNode.left())
      pNode.exchangeWithLeft();
    return;
  }

  // pNode has and left and right child
  if (pNode.self() < pNode.left() && pNode.left() > pNode.right()) {
    pNode.exchangeWithLeft();
    return drown(pNode.leftSubtree());
  }
  if (pNode.self() < pNode.right()) {
    pNode.exchangeWithRight();
    return drown(pNode.rightSubtree());
  }
  // Node is on it's place
}

void buildHeapTree(uint32_t* pArray, size_t nLength)
{
  if (nLength < 2)
    return;

  size_t i = (nLength - 2) / 2;
  do {
    drown(BinaryTreeNode(pArray, nLength, i));
  } while(i--);
}

void heapSort(uint32_t* pArray, size_t nLength)
{
  if (!nLength)
    return;

  buildHeapTree(pArray, nLength);
  for (size_t nTotalUnsorted = nLength; nTotalUnsorted > 1; --nTotalUnsorted) {
    std::swap(pArray[0], pArray[nTotalUnsorted - 1]);
    drown(BinaryTreeNode(pArray, nTotalUnsorted - 1));
  }
}

bool checkHeapTree(BinaryTreeNode pRoot)
{
  if (!pRoot.hasLeft())
    return true;

  if (pRoot.self() < pRoot.left() || !checkHeapTree(pRoot.leftSubtree()))
    return false;

  if (!pRoot.hasRight())
    return true;

  if (pRoot.self() < pRoot.right() || !checkHeapTree(pRoot.rightSubtree()))
    return false;

  return true;
}

bool TestBuildHeapTree()
{
  size_t    nLength = 10000;
  uint32_t* pArray = new uint32_t[nLength];
  for(uint32_t i = 0; i < nLength; ++i)
    pArray[i] = i;

  for(size_t i = 0; i < 100; ++i) {
    std::random_shuffle(pArray, pArray + nLength);
    buildHeapTree(pArray, nLength);
    if (!checkHeapTree(BinaryTreeNode(pArray, nLength)))
      return false;
  }
  return true;
}

bool TestHeapSort()
{
  size_t    nLength = 10000;
  uint32_t* pArray = new uint32_t[nLength];
  for(uint32_t i = 0; i < nLength; ++i)
    pArray[i] = i;

  for(size_t i = 0; i < 100; ++i) {
    std::random_shuffle(pArray, pArray + nLength);
    heapSort(pArray, nLength);
    for(uint32_t j = 0; j < nLength; ++j)
      if (pArray[j] != j)
        return false;
  }
  return true;
}

int main(int argc, char* argv[])
{
  std::srand(time(nullptr));

  if (argc == 1) {
    std::cout << "Testing buildHeapTree(): " << (TestBuildHeapTree() ? "OK" : "FAILED") << std::endl;
    std::cout << "Testing heapSort():      " << (TestHeapSort() ? "OK" : "FAILED") << std::endl;
    return 0;
  }
  return 0;
}
