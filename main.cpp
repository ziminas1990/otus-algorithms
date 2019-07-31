#include <memory>
#include <vector>
#include <iostream>

#include "Utils.h"
#include "IBucket.h"
#include "ListBucket.h"
#include "AVLTreeBucket.h"


template<typename Key, class Value, class Hash = std::hash<Key>>
class HashTable
{
  using Bucket    = IBucket<Key, Value>;
  using BucketPtr = std::unique_ptr<Bucket>;
  using Buckets   = std::vector<BucketPtr>;
public:

  HashTable(size_t nInitialBucketsCount = 101)
    : m_Buckets(nInitialBucketsCount), m_nTotalItems(0)
  {}

  void insert(Key const& key, Value value)
  {
    if (m_nTotalItems >= 2 * m_Buckets.size())
      expand();

    if (getOrCreateBucket(key)->insert(key, value))
      ++m_nTotalItems;
  }

  Value get(Key const& key)
  {
    BucketPtr& pBucket = getBucket(key);
    return pBucket ? pBucket->find(key) : Value();
  }

  bool remove(Key const& key)
  {
    BucketPtr& pBucket = getBucket(key);
    if (pBucket && pBucket->remove(key)) {
      --m_nTotalItems;
      return true;
    }
    return false;
  }

  size_t size() const { return m_nTotalItems; }

private:
  size_t getBucketId(Key const& key) const { return Hash()(key) % m_Buckets.size(); }

  BucketPtr& getBucket(Key const& key) { return m_Buckets.at(getBucketId(key)); }
  BucketPtr const& getBucket(Key const& key) const
  {
    return m_Buckets.at(getBucketId(key));
  }

  BucketPtr& getOrCreateBucket(Key const& key)
  {
    BucketPtr& pBucket = m_Buckets[getBucketId(key)];
    if (!pBucket)
      pBucket = std::make_unique<AVLTreeBucket<Key, Value>>();
    return pBucket;
  }

  void expand()
  {
    Buckets oldBuckets = std::move(m_Buckets);
    m_Buckets.resize(m_nTotalItems * 2);
    m_nTotalItems = 0;

    typename Bucket::VisitorFunction fInserter =
        [this](Key const& key, Value& value) { insert(key, std::move(value)); };
    for(BucketPtr& pOldBucket : oldBuckets)
      if (pOldBucket)
        pOldBucket->visitAll(fInserter);
  }

private:
  Buckets m_Buckets;
  size_t  m_nTotalItems;
};


bool TestHashTable()
{
  const size_t nTotal = 100000;
  HashTable<uint32_t, uint32_t> table;

  for (uint32_t i = 0; i < nTotal; ++i)
    table.insert(i, i * 2);

  if (table.size() != nTotal)
    return false;

  for (uint32_t i = 0; i < nTotal; ++i) {
    if (table.get(i) != (i * 2))
      return false;
  }

  std::vector<uint32_t> nRandomKeys = makeArray<uint32_t>(nTotal);
  size_t nTotalElementsToRemove = nTotal / 2;
  for (size_t i = 0; i < nTotalElementsToRemove; ++i) {
    if (!table.remove(nRandomKeys[i]))
      return false;
  }

  if (table.size() != nTotal - nTotalElementsToRemove)
    return false;

  for (size_t i = 0; i < nTotal; ++i) {
    bool lMatch = table.get(nRandomKeys[i]) == nRandomKeys[i] * 2;
    if (i < nTotalElementsToRemove && lMatch)
      return false;
    if (i >= nTotalElementsToRemove && !lMatch)
      return false;
  }
  return true;
}


int main(int, char*[])
{
  std::cout << "Testing HashTable: " << (TestHashTable() ? "OK" : "FAIL") << std::endl;
  return 0;
}
