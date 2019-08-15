#include "AwesomeHeap.h"
#include <vector>
#include <algorithm>  // for random_shuufle
#include <assert.h>
#include <iostream>
#include "Utils.h"

struct AllocatedChunk {
  uint16_t* pData           = nullptr;
  size_t    nLength         = 0;
  uint16_t  nExpectedValues = 0;
};

void allocateRandomChunks(std::vector<AllocatedChunk>& chunks, size_t nTotal,
                          AwesomeHeap& heap)
{
  for (size_t i = 0; i < nTotal; ++i) {
    AllocatedChunk& chunk = chunks[i];
    chunk.nExpectedValues = rand() % 0xFFFF;
    chunk.nLength         = rand() % 512;
    chunk.pData           = (uint16_t*)(heap.allocate(chunk.nLength * sizeof(uint16_t)));
    assert(chunk.pData);
    for (size_t j = 0; j < chunk.nLength; ++j)
      chunk.pData[j] = chunk.nExpectedValues;
  }
}

void freeRandomChunks(std::vector<AllocatedChunk>& chunks, size_t nTotal,
                      AwesomeHeap& heap)
{
  for (size_t j = 0; j < nTotal; ++j) {
    AllocatedChunk& chunk = chunks[j];
    heap.free(chunk.pData);
    chunk = AllocatedChunk();
  }
}

bool checkChunks(std::vector<AllocatedChunk> const& chunks)
{
  for (AllocatedChunk const& chunk : chunks)
    for (size_t j = 0; j < chunk.nLength; ++j)
      if (chunk.pData[j] != chunk.nExpectedValues)
        return false;
  return true;
}

bool Test(AwesomeHeap& heap)
{
  const size_t nTotalChunks = 1000;

  std::vector<AllocatedChunk> chunks;
  chunks.resize(nTotalChunks);
  allocateRandomChunks(chunks, nTotalChunks, heap);

  for(size_t i = 0; i < 1000; ++i) {
    std::random_shuffle(chunks.begin(), chunks.end());

    size_t nElementsToReallocate = std::rand() % (nTotalChunks / 2);
    freeRandomChunks(chunks, nElementsToReallocate, heap);
    allocateRandomChunks(chunks, nElementsToReallocate, heap);
    if (!checkChunks(chunks))
      return false;
    if (i % 100 == 0) {
      std::cout << "Iteration #" << i << ": " << heap.getFragmentationLevel() << std::endl;
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& out, AwesomeHeap::AllocatePolicy ePolicy)
{
  switch(ePolicy) {
    case AwesomeHeap::eBest: return out << "Best";
    case AwesomeHeap::eFirst: return out << "First";
    case AwesomeHeap::eWorst: return out << "Worst";
  }
}

int main(int argc, char* argv[])
{
  std::srand(time(nullptr));

  for(AwesomeHeap::AllocatePolicy policy : AwesomeHeap::AllPolicies())
  {
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "Testing AwesomeHeap with \"" << policy <<  "\" policy..." << std::endl;
    AwesomeHeap heap(1000 * 1024, policy);
    Stopwatch stopwatch;
    stopwatch.start();
    bool lSuccess = Test(heap);
    long duration = stopwatch.sinceStartMs();
    std::cout << "Test status: " << (lSuccess ? "OK" : "FAILED!") << " (in " <<
                 duration << " ms)" << std::endl;
  }
  return 0;
}
