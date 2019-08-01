#pragma once

#include <list>
#include <algorithm> // for std::find_if only

#include "IBucket.h"

template<typename Key, typename Value>
class ListBucket : public IBucket<Key, Value>
{
  using BaseClass = IBucket<Key, Value>;
  using Item      = std::pair<Key, Value>;
  using ItemsList = std::list<Item>;
public:

  // overrides from IBacket
  bool insert(Key const& key, Value value) override
  {
    auto I = findItem(key);
    if (I != m_Items.end()) {
      I->second = std::move(value);
      return false;
    } else {
      m_Items.push_back(std::make_pair(key, std::move(value)));
      return true;
    }
  }

  Value find(Key const& key) const override
  {
    auto I = findItem(key);
    return I != m_Items.end() ? I->second : Value();
  }

  bool remove(Key const& key) override
  {
    auto I = findItem(key);
    if (I == m_Items.end())
      return false;
    m_Items.erase(I);
    return true;
  }

  void visitAll(typename BaseClass::VisitorFunction& fVisitor) override
  {
    for (Item& item : m_Items)
      fVisitor(item.first, item.second);
  }

  size_t totalItems() const override { return m_Items.size(); }

private:
  typename ItemsList::const_iterator findItem(Key const& key) const {
    return std::find_if(m_Items.begin(), m_Items.end(),
                        [&key](Item const& item) { return item.first == key; });
  }
  typename ItemsList::iterator findItem(Key const& key) {
    return std::find_if(m_Items.begin(), m_Items.end(),
                        [&key](Item const& item) { return item.first == key; });
  }

private:
  ItemsList m_Items;
};
