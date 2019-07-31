#pragma once

#include <functional>
#include <memory>
#include <stdlib.h>

template<typename Key, typename Value>
class IBucket
{
public:
  using VisitorFunction = std::function<void(Key const& key, Value& value)>;

  virtual ~IBucket() = default;

  // returns true, if element was inserted, or false, if existing element was replaced
  virtual bool   insert(Key const& key, Value value) = 0;
  virtual Value  find(Key const& key) const          = 0;
  virtual bool   remove(Key const& key)              = 0;
  virtual void   visitAll(VisitorFunction& fVisitor) = 0;
};

template<typename Key, typename Value>
using IBucketPtr = std::unique_ptr<IBucket<Key, Value>>;
