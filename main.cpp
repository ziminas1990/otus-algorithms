#include "Queue.h"
#include "PriorityQueue.h"
#include <vector>
#include <iostream>

bool TestQueue()
{
  Queue<size_t> queue;
  if (!queue.empty())
    return false;

  for(size_t i = 0; i < 50; ++i)
    queue.push(i);

  if (queue.empty())
    return false;

  for(size_t i = 0; i < 50; ++i)
    if (queue.pop() != i)
      return false;

  return queue.empty();
}

bool TestPriorityQueue()
{
  // Fill queue like this:
  // Priority  Values
  // 1         1 2
  // 2         3 4 5
  // 3         6
  // 10        7
  // 100       8 9

  std::vector<std::pair<uint8_t, uint8_t>> input = {
    {2, 3}, { 10, 7 }, {3, 6}, {1, 1}, {1, 2}, {2, 4}, {100, 8}, {100, 9}, {2, 5}
  };

  PriorityQueue<size_t> queue;
  for(size_t i = 0; i < 3; ++i) {
    for(std::pair<uint8_t, uint8_t> item : input)
      queue.enqueue(item.first, item.second);

    for(size_t j = 1; j <= 9; ++j)
      if (queue.dequeue() != j)
        return false;
  }

  return true;
}

int main(int argc, char* argv[])
{
  std::cout << "Queue:         " << (TestQueue()         ? "OK" : "FAILED") << std::endl;
  std::cout << "PriorityQueue: " << (TestPriorityQueue() ? "OK" : "FAILED") << std::endl;
  return 0;
}
