#include <memory>
#include <vector>
#include <iostream>

#include "Utils.h"
#include "IBucket.h"
#include "ListBucket.h"
#include "AVLTreeBucket.h"

enum class BucketsPolicy {
  eListBucketsOnly = 1,
  eAVLBucketsOnly  = 2,
  eMixedBuckets    = 3
};


template<typename Key, class Value,
         bool lSlidingSize = true,
         BucketsPolicy eBucketsPolicy = BucketsPolicy::eMixedBuckets,
         class Hash = std::hash<Key>>
class HashTable
{
  using Bucket    = IBucket<Key, Value>;
  using BucketPtr = std::unique_ptr<Bucket>;
  using Buckets   = std::vector<BucketPtr>;
public:

  HashTable(size_t nInitialBucketsCount = 101, size_t nListBucketLimit = 32)
    : m_Buckets(nInitialBucketsCount), m_nTotalItems(0),
      m_nListBucketLimit(nListBucketLimit)
  {}

  void insert(Key const& key, Value value)
  {
    if (m_nTotalItems >= 2 * m_Buckets.size())
      resize(m_nTotalItems * 2);

    BucketPtr& pBucket = getOrCreateBucket(key);
    if (!pBucket->insert(key, value))
      return;
    ++m_nTotalItems;
    if (m_nListBucketLimit && pBucket->totalItems() == m_nListBucketLimit) {
      replaceBucket(pBucket);
    }
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
      if (3 * m_nTotalItems < m_Buckets.size())
        resize(m_nTotalItems / 3);
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
    if (!pBucket) {
      if constexpr(eBucketsPolicy == BucketsPolicy::eAVLBucketsOnly) {
        pBucket = std::make_unique<AVLTreeBucket<Key, Value>>();
      } else {
        pBucket = std::make_unique<ListBucket<Key, Value>>();
      }
    }
    return pBucket;
  }

  void resize(size_t nTotalBuckets)
  {
    if constexpr(!lSlidingSize)
      return;

    Buckets oldBuckets = std::move(m_Buckets);
    m_Buckets.resize(nTotalBuckets);
    m_nTotalItems = 0;

    typename Bucket::VisitorFunction fInserter =
        [this](Key const& key, Value& value) { insert(key, std::move(value)); };
    for(BucketPtr& pOldBucket : oldBuckets)
      if (pOldBucket)
        pOldBucket->visitAll(fInserter);
  }

  void replaceBucket(BucketPtr& pBucket)
  {
    if constexpr(eBucketsPolicy != BucketsPolicy::eMixedBuckets)
      return;

    // Creating new bucket, powered by AVL Tree and moving data from old bucket
    BucketPtr pOldBucket = std::move(pBucket);
    pBucket = std::make_unique<AVLTreeBucket<Key, Value>>();

    typename Bucket::VisitorFunction fInserter =
        [&pBucket](Key const& key, Value& value) {
          pBucket->insert(key, std::move(value));
        };
    pOldBucket->visitAll(fInserter);
  }

private:
  Buckets m_Buckets;
  size_t  m_nTotalItems;
  size_t  m_nListBucketLimit;
};


template<bool lSlidingSize, BucketsPolicy eBucketsPolicy>
bool TestHashTable(size_t nTotal)
{
  HashTable<uint32_t, uint32_t, lSlidingSize, eBucketsPolicy> table;

  std::vector<uint32_t> randomKeys = makeArray<uint32_t>(nTotal);
  for (uint32_t key : randomKeys)
    table.insert(key, key * 2 + 1);

  if (table.size() != nTotal)
    return false;

  for (uint32_t i = 0; i < nTotal; ++i) {
    if (table.get(i) != (i * 2 + 1))
      return false;
  }

  randomKeys = makeArray<uint32_t>(nTotal);
  size_t nTotalElementsToRemove = nTotal / 2;
  for (size_t i = 0; i < nTotalElementsToRemove; ++i) {
    if (!table.remove(randomKeys[i]))
      return false;
  }

  if (table.size() != nTotal - nTotalElementsToRemove)
    return false;

  for (size_t i = 0; i < nTotal; ++i) {
    bool lMatch = table.get(randomKeys[i]) == randomKeys[i] * 2 + 1;
    if (i < nTotalElementsToRemove && lMatch)
      return false;
    if (i >= nTotalElementsToRemove && !lMatch)
      return false;
  }
  return true;
}

bool runHashTableTests()
{
  return TestHashTable<false, BucketsPolicy::eListBucketsOnly>(5000)
      && TestHashTable<false, BucketsPolicy::eAVLBucketsOnly>(25000)
      && TestHashTable<false, BucketsPolicy::eMixedBuckets>(25000)
      && TestHashTable<true,  BucketsPolicy::eListBucketsOnly>(25000)
      && TestHashTable<true,  BucketsPolicy::eAVLBucketsOnly>(25000)
      && TestHashTable<true,  BucketsPolicy::eMixedBuckets>(25000);
}

template<bool lSlidingSize, BucketsPolicy eBucketsPolicy>
long CheckPerfomance()
{
  Stopwatch stopwatch;
  stopwatch.start();
  TestHashTable<lSlidingSize, eBucketsPolicy>(300000);
  return stopwatch.sinceStartMs();
}

int main(int, char*[])
{
  std::cout << "Testing HashTable: " <<
            RunRandomizedTest(runHashTableTests, time(nullptr), 50) << std::endl;

  std::cout << "Fixed,   ListBuckets:  " <<
               CheckPerfomance<false, BucketsPolicy::eListBucketsOnly>() << std::endl;
  std::cout << "Fixed,   MixedBuckets: " <<
               CheckPerfomance<false, BucketsPolicy::eMixedBuckets>() << std::endl;
  std::cout << "Fixed,   AVLBuckets:   " <<
               CheckPerfomance<false, BucketsPolicy::eAVLBucketsOnly>() << std::endl;
  std::cout << "Sliding, ListBuckets:  " <<
               CheckPerfomance<true, BucketsPolicy::eListBucketsOnly>() << std::endl;
  std::cout << "Sliding, MixedBuckets: " <<
               CheckPerfomance<true, BucketsPolicy::eMixedBuckets>() << std::endl;
  std::cout << "Sliding, AVLBuckets:   " <<
               CheckPerfomance<true, BucketsPolicy::eAVLBucketsOnly>() << std::endl;
  return 0;
}
