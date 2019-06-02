#include <iostream>
#include <stdint.h>
#include <string>
#include "Stopwatch.h"

using Implementation = uint64_t(uint64_t a, uint64_t b);

uint64_t euklid_naive(uint64_t a, uint64_t b)
{
  while (a != b) {
    if (a > b)
      a -= b;
    else
      b -= a;
  }
  return a;
}

uint64_t euklid_optimized(uint64_t a, uint64_t b)
{
  while (a && b) {
    if (a > b)
      a %= b;
    else
      b %= a;
  }
  return a + b;
}

bool Test(Implementation *pFunc)
{
  if (pFunc(75, 45) != 15)
    return false;
  if (pFunc(66, 24) != 6)
    return false;
  return true;
}

void printHelp(std::string const& selfApp)
{
  std::cout << "Help:\n";
  std::cout << "    " << selfApp << " <string: naive/optimized> <long: a> <long: b>\n";
  std::cout << "Examples:\n";
  std::cout << "    " << selfApp << " naive 666 24\n";
  std::cout << "    " << selfApp << " optimized 144 36" << std::endl;

}

int main(int argc, char* argv[])
{
  if (argc != 4) {
    printHelp(argv[0]);
    return 1;
  }

  Implementation* fImpl;

  if (std::string(argv[1]) == "naive") {
    fImpl = &euklid_naive;
  } else if (std::string(argv[1]) == "optimized"){
    fImpl = &euklid_optimized;
  } else {
    printHelp(argv[0]);
    return 1;
  }

  std::cout << "Testing: ";
  if (!Test(fImpl)) {
    std::cout << "FAILED" << std::endl;
    return 1;
  } else {
    std::cout << "OK" << std::endl;
  }

  uint64_t a = atoll(argv[2]);
  uint64_t b = atoll(argv[3]);

  Stopwatch stopwatch;
  stopwatch.start();
  uint64_t NOD = fImpl(a, b);
  long time = stopwatch.sinceStartMs();

  std::cout << "Result: " << NOD << std::endl;
  std::cout << "Time:   " << time << " ms" << std::endl;
  return 0;
}
