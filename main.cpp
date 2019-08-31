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
      pFirst->pPrevios = pLine;
      pFirst           = pLine;
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
    : pDownLevel(pDownLevel), nSystemBusWidth(nSystemBusWidth), nMaxLines(nMaxLines),
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

    bool lCacheHit   = false;
    CacheLine* pLine = getOrLoadCacheLine(nPageAddr, lCacheHit);

    if (!lCacheHit) {
      pDownLevel->load(nPageAddr, pLine->pPage, nSystemBusWidth);
      nLastOperationTime += pDownLevel->lastOperationTime();
    } else {
      nLastOperationTime += nTicksToRead;
    }

    moveToFirst(pLine);
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
      CacheLine* pLine   = I->second;
      pLine[nPageOffset] = value;
      pLine->lModified   = true;
      nLastOperationTime = nTicksToWrite;
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

  CacheLine* getOrLoadCacheLine(size_t nPageAddr, bool& lCacheHit)
  {
    auto I    = pageAddrToLine.find(nPageAddr);
    lCacheHit = (I != pageAddrToLine.end());

    if (!lCacheHit)
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

    pLine->nPageAddr = nPageAddr;
    pLine->lModified = false;
    pDownLevel->load(nPageAddr, pLine->pPage, nSystemBusWidth);

    cacheLines.pushToHead(pLine);
    return pLine;
  }

  void flushCacheLine(CacheLine* pLine) {
    pDownLevel->flush(pLine->nPageAddr, pLine->pPage, nSystemBusWidth);
    nLastOperationTime += pDownLevel->lastOperationTime();
  }

private:
  AbstractRAM* pDownLevel;

  size_t nSystemBusWidth;
  size_t nMaxLines;
  size_t nTicksToRead;
  size_t nTicksToWrite;
  size_t nLastOperationTime;

  CacheLineList cacheLines;
  std::unordered_map<size_t, CacheLine*> pageAddrToLine;
};


bool RandomReadWriteTest(AbstractRAM* pRAM, size_t nAvaliable)
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


int main(int argc, char* argv[])
{
  size_t nCapacity = 100;
  RAM justRAM(nCapacity, 10, 5);
  LRUCache cache(&justRAM, 4, 8, 2, 4);

  auto fReadWriteTest = [&]() { return RandomReadWriteTest(&cache, nCapacity); };

  std::cout << "Testing RAM: " <<
               RunRandomizedTest(fReadWriteTest, 59774, 100) << std::endl;

  return 0;
}
