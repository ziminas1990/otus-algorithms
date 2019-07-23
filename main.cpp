#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <set>

#include "Stopwatch.h"
#include "TreeNode.h"


template<typename T>
class AVLTree
{
public:
  AVLTree() : pRoot(nullptr) {}

  void insert(T value) {
    if (pRoot) {
      pRoot->insert(std::move(value));
    } else {
      pRoot = new TreeNode<T>(std::move(value));
    }
  }

  bool remove(T const& value) {
    if (!pRoot)
      return false;
    if (pRoot->data() != value || !pRoot->isLeaf()) {
      return pRoot->remove(value);
    }
    // pRoot is leaf and contain exact value
    delete pRoot;
    pRoot = nullptr;
    return true;
  }

  bool has(T const& value) const { return pRoot && pRoot->find(value); }

  bool isBalanced() const { return pRoot->isBalanced(); }

  void mixedTraversal(std::vector<T>& out) const
  {
    if (!pRoot)
      return;
    // optimization:
    size_t nInitialCapacity = 1 << pRoot->level();
    if (nInitialCapacity > 2048)
      nInitialCapacity = 2048;
    out.reserve(nInitialCapacity);

    pRoot->mixedTraversal(out);
  }

private:
  TreeNode<T>* pRoot;
};


bool TestInsert()
{
  // 1. Creating random array
  size_t nTotal = 10000;
  std::vector<uint16_t> elements(nTotal);
  for(size_t i = 0; i < nTotal; ++i)
    elements[i] = i;
  std::random_shuffle(elements.begin(), elements.end());

  // 2. Building AVL tree, using elements from array
  AVLTree<uint16_t> tree;
  for(uint16_t element : elements) {
    tree.insert(element);
    if (!tree.isBalanced())
      return false;
  }

  // 3. creating new array by mixed traversing AVL tree
  elements.clear();
  tree.mixedTraversal(elements);

  // 4. checking, that new array is sorted
  for (size_t i = 0; i < nTotal; ++i)
    if (elements[i] != i)
      return false;
  return true;
}

bool TestRemove()
{
  // 1. Creating random array
  size_t nTotal = 10000;
  std::vector<uint16_t> elements(nTotal);
  for(uint16_t i = 0; i < nTotal; ++i)
    elements[i] = i;
  std::random_shuffle(elements.begin(), elements.end());

  // 2. selecting some elements, that will be removed
  std::set<uint16_t> elementsToRemove;
  for(uint16_t i = 0; i < nTotal / 10 + 1; ++i)
    elementsToRemove.insert(elements[i]);
  std::random_shuffle(elements.begin(), elements.end());

  // 3. Building AVL tree, using elements from array
  AVLTree<uint16_t> tree;
  for(uint16_t element : elements) {
    tree.insert(element);
    if (!tree.isBalanced())
      return false;
  }

  // 4. Removing elements from tree
  for (uint16_t element : elementsToRemove) {
    tree.remove(element);
    if (!tree.isBalanced())
      return false;
  }

  // 5. creating new array by mixed traversing AVL tree and checking that it is sorted
  elements.clear();
  tree.mixedTraversal(elements);
  if (elements.size() != nTotal - elementsToRemove.size())
    return false;
  for (size_t i = 1; i < elements.size(); ++i)
    if (elements[i] < elements[i - 1])
      return false;

  // 6. Checking, that there is no removed elements in array
  for (uint16_t element : elements)
    if (elementsToRemove.find(element) != elementsToRemove.end())
      return false;

  return true;
}


int main(int argc, char* argv[])
{
  std::srand(time(nullptr));

  std::cout << "Testing insertions: " << (TestInsert() ? "OK" : "FAILED") << std::endl;
  std::cout << "Testing remove:     " << (TestRemove() ? "OK" : "FAILED") << std::endl;
  return 0;
}
