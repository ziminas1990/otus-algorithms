#include <vector>
#include <algorithm>  // for random_shuffle
#include <assert.h>
#include <iostream>
#include <memory.h>

#include <list>
#include <unordered_map>

#include "Utils.h"

class AbstractRAM
{
public:
  virtual ~AbstractRAM() = default;

  virtual uint8_t get(size_t addr) = 0;
  virtual void    set(size_t addr, uint8_t value) = 0;
  virtual void    load(size_t addr, uint8_t* pDest, size_t nBusWidth) = 0;
  virtual void    flush(size_t addr, uint8_t const* pSource, size_t nBusWidth) = 0;
  virtual size_t  lastOperationTime() const = 0;
};


class RAM : public AbstractRAM
{
public:
  RAM(size_t size, size_t nTicksToRead, size_t nTicksToWrite)
    : pArena(new uint8_t[size]), nTicksToRead(nTicksToRead), nTicksToWrite(nTicksToWrite)
  {}

  ~RAM() override { delete [] pArena; }

protected:
  uint8_t get(size_t addr) override
  {
    nLastOperationTime = nTicksToRead;
    return pArena[addr];
  }

  void set(size_t addr, uint8_t value) override
  {
    nLastOperationTime = nTicksToWrite;
    pArena[addr] = value;
  }

  void load(size_t addr, uint8_t* pDest, size_t nBusWidth) override
  {
    nLastOperationTime = nTicksToRead;
    memcpy(pDest, pArena + addr, nBusWidth);
  }

  void flush(size_t addr, uint8_t const* pSource, size_t nBusWidth)
  {
    nLastOperationTime = nTicksToWrite;
    memcpy(pArena + addr, pSource, nBusWidth);
  }

  size_t lastOperationTime() const { return nLastOperationTime; }

private:
  uint8_t* pArena = nullptr;
  size_t   nTicksToRead;
  size_t   nTicksToWrite;
  size_t   nLastOperationTime;
};


class LRUCache : public AbstractRAM
{
  struct CacheLine {
    CacheLine(size_t nCacheLineSize)
      : pPage(new uint8_t[nCacheLineSize])
    {}
    ~CacheLine() { delete[] pPage; }

    void excludeSelf() {
      if (pNext)
        pNext->pPrevios = pPrevios;
      if (pPrevios)
        pPrevios->pNext = pNext;
      pNext    = nullptr;
      pPrevios = nullptr;
    }

    uint8_t*   pPage     = nullptr;
    size_t     nPageAddr = 0;
    bool       lModified = false;
    CacheLine* pNext     = nullptr;
    CacheLine* pPrevios  = nullptr;
  };

  struct CacheLineList {

    void push(CacheLine* pLine) {
      if (pLast) {
        pLast->pNext    = pLine;
        pLine->pPrevios = pLast;
        pLine->pNext    = nullptr;
      }
      pLast = pLine;
      if (!pFirst)
        pFirst = pLast;
    }

    void pushToHead(CacheLine* pLine) {
      if (pLine == pFirst)
        return;
      pLine->pPrevios  = nullptr;
      pLine->pNext     = pFirst;
      if (pFirst)
        pFirst->pPrevios = pLine;
      pFirst = pLine;
      if (!pLast)
        pLast = pFirst;
    }

    void exclude(CacheLine* pLine) {
      if (pLine == pFirst)
        pFirst = pFirst->pNext;
      if (pLine == pLast)
        pLast = pLast->pPrevios;
      pLine->excludeSelf();
    }

    CacheLine* popTail() {
      CacheLine* pLine = pLast;
      exclude(pLine);
      return pLine;
    }

    CacheLine* pFirst = nullptr;
    CacheLine* pLast  = nullptr;
  };

public:
  LRUCache(AbstractRAM* pDownLevel, size_t nSystemBusWidth, size_t nMaxLines,
           size_t nTicksToRead, size_t nTicksToWrite)
    : pDownLevel(pDownLevel), nSystemBusWidth(nSystemBusWidth),
      nTicksToRead(nTicksToRead), nTicksToWrite(nTicksToWrite)
  {
    for(size_t i = 1; i < nMaxLines; ++i)
      cacheLines.push(new CacheLine(nSystemBusWidth));
  }

  uint8_t get(size_t addr) override
  {
    nLastOperationTime = 0;
    size_t nPageOffset = addr % nSystemBusWidth;
    size_t nPageAddr   = addr - nPageOffset;

    CacheLine* pLine   = getOrLoadCacheLine(nPageAddr);
    nLastOperationTime += nTicksToRead;
    return pLine->pPage[nPageOffset];
  }

  void set(size_t addr, uint8_t value) override
  {
    nLastOperationTime     = 0;
    size_t     nPageOffset = addr % nSystemBusWidth;
    size_t     nPageAddr   = addr - nPageOffset;

    auto I = pageAddrToLine.find(nPageAddr);
    if (I == pageAddrToLine.end()) {
      pDownLevel->set(addr, value);
      nLastOperationTime += pDownLevel->lastOperationTime();
    } else {
      CacheLine* pLine          = I->second;
      pLine->pPage[nPageOffset] = value;
      pLine->lModified          = true;
      nLastOperationTime        = nTicksToWrite;
    }
  }

  void load(size_t addr, uint8_t* pDest, size_t nBusWidth) override
  {
    pDownLevel->load(addr, pDest, nBusWidth);
    nLastOperationTime = pDownLevel->lastOperationTime();
  }

  void flush(size_t addr, uint8_t const* pSource, size_t nBusWidth)
  {
    pDownLevel->flush(addr, pSource, nBusWidth);
    nLastOperationTime = pDownLevel->lastOperationTime();
  }

  size_t lastOperationTime() const { return nLastOperationTime; }

private:

  CacheLine* getOrLoadCacheLine(size_t nPageAddr)
  {
    auto I = pageAddrToLine.find(nPageAddr);  // ~O(1)
    if (I == pageAddrToLine.end())
      return loadToCache(nPageAddr);

    CacheLine* pLine = I->second;
    cacheLines.exclude(pLine);
    cacheLines.pushToHead(pLine);
    return pLine;
  }

  CacheLine* loadToCache(size_t nPageAddr) {
    CacheLine* pLine = cacheLines.popTail();
    if (pLine->lModified)
      flushCacheLine(pLine);
    pageAddrToLine.erase(pLine->nPageAddr);

    pLine->nPageAddr = nPageAddr;
    pLine->lModified = false;
    pDownLevel->load(nPageAddr, pLine->pPage, nSystemBusWidth);
    nLastOperationTime += pDownLevel->lastOperationTime();

    cacheLines.pushToHead(pLine);
    pageAddrToLine[nPageAddr] = pLine;
    return pLine;
  }

  void flushCacheLine(CacheLine* pLine) {
    pDownLevel->flush(pLine->nPageAddr, pLine->pPage, nSystemBusWidth);
    nLastOperationTime += pDownLevel->lastOperationTime();
  }

private:
  AbstractRAM* pDownLevel;

  size_t nSystemBusWidth;
  size_t nTicksToRead;
  size_t nTicksToWrite;
  size_t nLastOperationTime;

  CacheLineList cacheLines;
  std::unordered_map<size_t, CacheLine*> pageAddrToLine;
};


bool SimpleTest(AbstractRAM* pRAM, size_t nAvaliable)
{
  struct Cell {
    Cell() : addr(0), nValue(0) {}
    Cell(size_t addr, uint8_t nValue)
      : addr(addr), nValue(nValue)
    {}
    size_t  addr;
    uint8_t nValue;
  };

  std::vector<Cell> data(nAvaliable);
  for(size_t i = 0; i < nAvaliable; ++i)
    data[i] = Cell(i, std::rand() % 0xFF);

  std::random_shuffle(data.begin(), data.end());
  for (Cell const& cell : data)
    pRAM->set(cell.addr, cell.nValue);

  std::random_shuffle(data.begin(), data.end());
  for (size_t i = 0; i < data.size(); ++i)
    if (data[i].nValue != pRAM->get(data[i].addr))
      return false;
  return true;
}

inline
size_t RandomRead(AbstractRAM* pRAM, size_t nTotalOperations,
                  size_t nBeginAddr, size_t nEndAddr)
{
  size_t nTotalTicks = 0;
  for(size_t i = 0; i < nTotalOperations; ++i) {
    pRAM->get(randomNumber(nBeginAddr, nEndAddr));
    nTotalTicks += pRAM->lastOperationTime();
  }
  return nTotalTicks;
}

inline
size_t RandomWrite(AbstractRAM* pRAM, size_t nTotalOperations,
                   size_t nBeginAddr, size_t nEndAddr)
{
  size_t nTotalTicks = 0;
  for(size_t i = 0; i < nTotalOperations; ++i) {
    pRAM->set(randomNumber(nBeginAddr, nEndAddr), std::rand() % 0xFF);
    nTotalTicks += pRAM->lastOperationTime();
  }
  return nTotalTicks;
}

inline size_t Scanning(AbstractRAM* pRAM, size_t nBeginAddr, size_t nEndAddr, bool lRead)
{
  bool lForward = nEndAddr > nBeginAddr;
  size_t nTotalTicks = 0;
  for(size_t nAddr = nBeginAddr; nAddr != nEndAddr; nAddr += (lForward) ? 1 : -1) {
    if (lRead) {
      pRAM->get(nAddr);
    } else {
      pRAM->set(nAddr, std::rand() % 0xFF);
    }
    nTotalTicks += pRAM->lastOperationTime();
  }
  return nTotalTicks;
}

size_t RandomReadWrite(AbstractRAM* pRAM, size_t nTotalOperations,
                       size_t nBeginAddr, size_t nEndAddr, uint8_t nReadWriteFactor)
{
  size_t nTotalTicks = 0;
  while (nTotalOperations) {
    size_t nOperations = std::rand() % 20;

    if (nOperations > nTotalOperations)
      nOperations = nTotalOperations;
    nTotalOperations -= nOperations;

    bool lRead = std::rand() % (1 + nReadWriteFactor);
    if (lRead) {
      nTotalTicks += RandomRead(pRAM, nOperations, nBeginAddr, nEndAddr);
    } else {
      nTotalTicks += RandomWrite(pRAM, nOperations, nBeginAddr, nEndAddr);
    }
  }
  return nTotalTicks;
}

size_t PartialyRandomReadWrite(AbstractRAM* pRAM, size_t nTotalOperations,
                               size_t nCapacity, uint8_t nRWFactor)
{
  size_t nTotalTicks = 0;
  while (nTotalOperations) {
    size_t nOperations = std::rand() % 1000;

    if (nOperations > nTotalOperations)
      nOperations = nTotalOperations;
    nTotalOperations -= nOperations;

    size_t nAreaSize     = 2048;
    size_t nBeginAddress = randomNumber(0, nCapacity - nAreaSize);
    size_t nEndAddress   = nBeginAddress + nAreaSize;

    nTotalTicks += RandomReadWrite(pRAM, nOperations, nBeginAddress, nEndAddress, nRWFactor);
  }
  return nTotalTicks;
}

size_t RandomScanningTest(AbstractRAM* pRAM, size_t nTotalOperations,
                          size_t nCapacity, uint8_t nRWFactor)
{
  size_t nTotalTicks = 0;
  while (nTotalOperations) {
    size_t nOperations = std::rand() % 1000;

    if (nOperations > nTotalOperations)
      nOperations = nTotalOperations;
    nTotalOperations -= nOperations;

    size_t nAreaSize     = 2048;
    size_t nBeginAddress = randomNumber(nAreaSize, nCapacity - nAreaSize);
    size_t nEndAddress   = randomNumber(nBeginAddress - nAreaSize,
                                        nBeginAddress + nAreaSize);
    bool lRead           = std::rand() % (1 + nRWFactor);
    nTotalTicks += Scanning(pRAM, nBeginAddress, nEndAddress, lRead);
  }
  return nTotalTicks;
}

int main(int argc, char* argv[])
{
  if (argc < 2)
    return 0;
  std::srand(time(nullptr));

  std::string sMode = argv[1];

  if (sMode == "autotest") {
    size_t nCapacity = 16 * 1024;
    RAM justRAM(nCapacity, 5, 10);
    LRUCache cache(&justRAM, 4, 2, 2, 4);

    auto fReadWriteTest = [&]() { return SimpleTest(&cache, nCapacity); };
    std::cout << "LRUCache random R/W test: " <<
                 RunRandomizedTest(fReadWriteTest, time(nullptr), 100) << std::endl;
    return 0;
  }

  if (sMode == "perfomance") {
    std::vector<size_t> cacheLinesSize  = { 2, 4,  8,  16, 32,  64 };
    std::vector<size_t> cacheTotalLines = { 8, 16, 32, 64, 128, 256 };

    uint16_t nSeed   = std::rand() % 0xFFFF;
    size_t nCapacity = 128 * 1024;

    std::cout << "Random R/W test:" << std::endl;
    for (size_t cacheLines : cacheTotalLines){
      std::srand(nSeed);
      std::cout << cacheLines << std::flush;

      for (size_t cacheLineSize : cacheLinesSize) {
        RAM justRAM(nCapacity, 20, 20);
        LRUCache cache(&justRAM, cacheLineSize, cacheLines, 1, 1);
        size_t nTicks = RandomReadWrite(&cache, 100000, 0, nCapacity - 1, 4);
        std::cout << " " << (nTicks / 1000) << std::flush;
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Partially random R/W test:" << std::endl;
    for (size_t cacheLines : cacheTotalLines){
      std::srand(nSeed);
      std::cout << cacheLines << std::flush;

      for (size_t cacheLineSize : cacheLinesSize) {
        RAM justRAM(nCapacity, 20, 20);
        LRUCache cache(&justRAM, cacheLineSize, cacheLines, 1, 1);
        size_t nTicks = PartialyRandomReadWrite(&cache, 100000, nCapacity, 4);
        std::cout << " " << (nTicks / 1000) << std::flush;
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Random scanning R/W test:" << std::endl;
    for (size_t cacheLines : cacheTotalLines){
      std::srand(nSeed);
      std::cout << cacheLines << std::flush;

      for (size_t cacheLineSize : cacheLinesSize) {
        RAM justRAM(nCapacity, 20, 20);
        LRUCache cache(&justRAM, cacheLineSize, cacheLines, 1, 1);
        size_t nTicks = RandomScanningTest(&cache, 100000, nCapacity, 4);
        std::cout << " " << (nTicks / 1000) << std::flush;
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
  return 0;
}
