#pragma once

#include "AVLTreeNode.h"

#include "IBucket.h"

template<typename Key, typename Value>
class AVLTreeBucket : public IBucket<Key, Value>
{
  using BaseClass = IBucket<Key, Value>;

  struct NodeValue {
    NodeValue() = default;
    NodeValue(Key const& key) : key(key) {}
    NodeValue(Key const& key, Value value) : key(key), value(std::move(value)) {}

    bool operator< (NodeValue const& other) const { return key <  other.key; }
    bool operator<=(NodeValue const& other) const { return key <= other.key; }
    bool operator==(NodeValue const& other) const { return key == other.key; }
    bool operator!=(NodeValue const& other) const { return key != other.key; }

    Key   key;
    Value value;
  };

public:

  AVLTreeBucket() : pRoot(nullptr), m_nTotalItems(0) {}

  // overrides from IBacket
  bool insert(Key const& key, Value value) override
  {
    if (pRoot) {
      AVLTreeNode<NodeValue>* pNode = pRoot->find(NodeValue(key));
      if (pNode) {
        pNode->data().value = std::move(value);
        return false;
      }
      pRoot = pRoot->insert(NodeValue(key, value));
    } else {
      pRoot = new AVLTreeNode<NodeValue>(NodeValue(key, std::move(value)));
    }
    ++m_nTotalItems;
    return true;
  }

  Value find(Key const& key) const override
  {
    AVLTreeNode<NodeValue>* pNode = pRoot ? pRoot->find(NodeValue(key)) : nullptr;
    return pNode ? pNode->data().value : Value();
  }

  bool remove(Key const& key) override
  {
    if (!pRoot)
      return false;
    bool lHasElement = pRoot->find(NodeValue(key)) != nullptr;
    if (!lHasElement)
      return false;
    pRoot = pRoot->remove(NodeValue(key));
    --m_nTotalItems;
    return true;
  }

  void visitAll(typename BaseClass::VisitorFunction& fVisitor) override
  {
    if (!pRoot)
      return;
    pRoot->visit([&fVisitor](NodeValue& node) { fVisitor(node.key, node.value); });
  }

  size_t totalItems() const override { return m_nTotalItems; }

private:
  AVLTreeNode<NodeValue>* pRoot;
  size_t m_nTotalItems;
};
