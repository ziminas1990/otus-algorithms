#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <set>
#include <functional>

#include "Utils.h"
#include "RandomTreeNode.h"
#include "AVLTreeNode.h"

template<typename T, template<typename> class NodeType>
class Tree
{
public:
  Tree() : pRoot(nullptr) {}

  void insert(T value) {
    if (pRoot) {
      pRoot = pRoot->insert(std::move(value));
    } else {
      pRoot = new NodeType<T>(std::move(value));
    }
  }

  void remove(T const& value) {
    if (pRoot)
      pRoot = pRoot->remove(value);
  }

  size_t level() const {
    return pRoot ? pRoot->level() : 0;
  }

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
  NodeType<T>* pRoot;
};


bool TestInsert()
{
  // 1. Creating random array
  size_t nTotal = 5000;
  std::vector<uint16_t> elements = makeArray<uint16_t>(nTotal);

  // 2. Building AVL tree, using elements from array
  Tree<uint16_t, RandomTreeNode> tree;
  for(uint16_t element : elements) {
    tree.insert(element);
  }

  // 3. creating new array by mixed traversing AVL tree
  elements.clear();
  tree.mixedTraversal(elements);

  // 4. checking, that new array is sorted
  if (elements.size() != nTotal)
    return false;
  for (size_t i = 0; i < nTotal; ++i)
    if (elements[i] != i)
      return false;
  return true;
}

bool TestRemove()
{
  // 1. Creating random array
  size_t nTotal = 5000;
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
  Tree<uint16_t, RandomTreeNode> tree;
  for(uint16_t element : elements) {
    tree.insert(element);
  }

  // 4. Removing elements from tree
  for (uint16_t element : elementsToRemove) {
    tree.remove(element);
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

void RunAllTests()
{
  std::cout << "Testing insertions: " <<
               RunRandomizedTest(TestInsert, std::rand() % 100000, 100) << std::endl;
  std::cout << "Testing remove: " <<
               RunRandomizedTest(TestRemove, std::rand() % 100000, 100) << std::endl;
}

int main(int argc, char* argv[])
{
  std::srand(time(nullptr));

  if (argc == 1) {
    RunAllTests();
    return 0;
  }

  uint32_t nTotalElements  = atoi(argv[1]);
  uint32_t nShuffledChunks = atoi(argv[2]);
  if (!nShuffledChunks)
    nShuffledChunks = 1;

  uint32_t nShuffledChunkSize = nTotalElements / nShuffledChunks;

  std::vector<uint32_t> elements;

  Stopwatch stopwatch;
  long      duration = 0;

  {
    Tree<uint16_t, RandomTreeNode> randomTree;
    std::cout << "Randomized Tree:" << std::endl;
    elements = makeArray<uint32_t>(nTotalElements, nShuffledChunkSize);
    stopwatch.start();
    for (uint32_t nElement : elements)
      randomTree.insert(nElement);
    duration = stopwatch.sinceStartMs();
    std::cout << "  Insertion: " << duration     << " ms" << std::endl;
    std::cout << "  Height:    " << randomTree.level() << std::endl;

    elements = makeArray<uint32_t>(nTotalElements, nShuffledChunkSize);
    stopwatch.start();
    for (uint32_t nElement : elements)
      randomTree.remove(nElement);
    duration = stopwatch.sinceStartMs();
    std::cout << "  Remove:    " << duration << " ms" << std::endl << std::endl;
  }

  {
    Tree<uint16_t, AVLTreeNode> AVLTree;
    std::cout << "AVL Tree:" << std::endl;
    elements = makeArray<uint32_t>(nTotalElements, nShuffledChunkSize);
    stopwatch.start();
    for (uint32_t nElement : elements)
      AVLTree.insert(nElement);
    duration = stopwatch.sinceStartMs();
    std::cout << "  Insertion: " << duration     << " ms" << std::endl;
    std::cout << "  Height:    " << AVLTree.level() << std::endl;

    elements = makeArray<uint32_t>(nTotalElements, nShuffledChunkSize);
    stopwatch.start();
    for (uint32_t nElement : elements)
      AVLTree.remove(nElement);
    duration = stopwatch.sinceStartMs();
    std::cout << "  Remove:    " << duration << " ms" << std::endl << std::endl;
  }

  return 0;
}
