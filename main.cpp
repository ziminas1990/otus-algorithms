#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <set>
#include <functional>

#include "Utils.h"
#include "SplayTreeNode.h"
#include "AVLTreeNode.h"

template<typename T, template<typename> class NodeType>
class Tree
{
  //static constexpr bool isSplayTree() { return decltype<NodeType}
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

  bool has(T const& value);

  void mixedTraversal(std::vector<T>& out) const
  {
    if (pRoot)
      pRoot->mixedTraversal(out);
  }

private:
  NodeType<T>* pRoot;
};

template<typename T, template<typename> class NodeType>
bool Tree<T, NodeType>::has(T const& value)
{
  if (!pRoot)
    return false;
  auto pNode = pRoot->find(value);
  if (std::is_same<NodeType<T>, SplayTreeNode<T>>::value && pNode) {
    pRoot = pNode;
  }
  return pNode != nullptr;
}

bool TestInsert()
{
  // 1. Creating random array
  size_t nTotal = 5000;
  std::vector<uint16_t> elements = makeArray<uint16_t>(nTotal);

  // 2. Building AVL tree, using elements from array
  Tree<uint16_t, SplayTreeNode> tree;
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

bool TestSearch()
{
  // 1. Creating random array
  size_t nTotal = 5000;
  std::vector<uint16_t> elements = makeArray<uint16_t>(nTotal);

  // 2. Building AVL tree, using elements from array
  Tree<uint16_t, SplayTreeNode> tree;
  for(uint16_t element : elements) {
    if (element % 3)
      tree.insert(element);
  }

  // 3. Searching elements in tree
  for(uint16_t element : elements) {
    if (element % 3) {
      if (!tree.has(element))
        return false;
    } else {
      if (tree.has(element))
        return false;
    }
  }
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
  Tree<uint16_t, SplayTreeNode> tree;
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
  std::cout << "Testing searching: " <<
               RunRandomizedTest(TestSearch, std::rand() % 100000, 100) << std::endl;
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

  uint32_t nTotalElements    = atoi(argv[1]);
  uint32_t nShuffleChunkSize = atoi(argv[2]);

  std::vector<uint32_t> elements = makeArray<uint32_t>(nTotalElements, nShuffleChunkSize);

  Stopwatch stopwatch;
  long      duration = 0;

  {
    Tree<uint16_t, SplayTreeNode> randomTree;
    std::cout << "Randomized Tree:" << std::endl;
    stopwatch.start();
    for (uint32_t nElement : elements)
      randomTree.insert(nElement);
    duration = stopwatch.sinceStartMs();
    std::cout << "  Insertion: " << duration     << " ms" << std::endl;

    elements = makeArray<uint32_t>(nTotalElements, nShuffleChunkSize);
    stopwatch.start();
    for (uint32_t nElement : elements)
      randomTree.remove(nElement);
    duration = stopwatch.sinceStartMs();
    std::cout << "  Remove:    " << duration << " ms" << std::endl << std::endl;
  }

  {
    Tree<uint16_t, AVLTreeNode> AVLTree;
    std::cout << "AVL Tree:" << std::endl;
    stopwatch.start();
    for (uint32_t nElement : elements)
      AVLTree.insert(nElement);
    duration = stopwatch.sinceStartMs();
    std::cout << "  Insertion: " << duration     << " ms" << std::endl;

    elements = makeArray<uint32_t>(nTotalElements, nShuffleChunkSize);
    stopwatch.start();
    for (uint32_t nElement : elements)
      AVLTree.remove(nElement);
    duration = stopwatch.sinceStartMs();
    std::cout << "  Remove:    " << duration << " ms" << std::endl << std::endl;
  }

  return 0;
}
