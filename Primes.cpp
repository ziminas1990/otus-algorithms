#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <math.h>
#include "Stopwatch.h"
#include "Bitarray.h"

using Implementation = uint32_t(uint32_t, std::vector<uint32_t>& primes);

uint32_t primes_naive(uint32_t N, std::vector<uint32_t>& primes)
{
  for(uint32_t number = 1; number <= N; ++number) {
    bool isPrime = true;
    for (uint32_t j = 2; j < number && isPrime; ++j) {
      if (number % j == 0)
        isPrime = false;
    }
    if (isPrime)
      primes.push_back(number);
  }
  return primes.size();
}

uint32_t primes_optimized(uint32_t N, std::vector<uint32_t>& primes)
{
  for(uint32_t number = 1; number <= N; ++number) {
    bool isPrime = number == 2 || number % 2 != 0;
    uint32_t sqrtNumber = sqrt(number);
    for (uint32_t j = 3; j <= sqrtNumber && isPrime; j += 2) {
      if (number % j == 0)
        isPrime = false;
    }
    if (isPrime)
      primes.push_back(number);
  }
  return primes.size();
}

uint32_t primes_eratosfen(uint32_t N, std::vector<uint32_t>& primes)
{
  if (!N)
    return 0;

  primes.push_back(1);
  Bitarray bits(N + 1);
  for(size_t i = 2; i <= N; ++i) {
    if (bits.test(i))
      continue;
    primes.push_back(i);
    for(size_t j = i * i; j <= N; j += i)
      bits.set(j);
  }
  return primes.size();
}

bool Test(Implementation *pFunc)
{
  // compare with naive implementation
  for (size_t i = 1; i < 10000; i *= 2) {
    std::vector<uint32_t> expected;
    std::vector<uint32_t> actual;
    if (primes_naive(i, expected) != pFunc(i, actual))
      return false;
    if (expected != actual)
      return false;
  }
  return true;
}

void printHelp(std::string const& selfApp)
{
  std::cout << "Help:\n";
  std::cout << "    " << selfApp << " <string: naive/optimized/eratosfen> <long: N>\n";
  std::cout << "Examples:\n";
  std::cout << "    " << selfApp << " naive     100000\n";
  std::cout << "    " << selfApp << " optimized 6000000\n";
  std::cout << "    " << selfApp << " eratosfen 6000000" << std::endl;
}

int main(int argc, char* argv[])
{
  std::cout << "Testing Bitsarray:        " << (TestBitArray() ? "OK" : "FAILED") << std::endl;
  std::cout << "Testing primes_optimized: " <<
               (Test(&primes_optimized) ? "OK" : "FAILED") << std::endl;
  std::cout << "Testing primes_eratosfen: " <<
               (Test(&primes_eratosfen) ? "OK" : "FAILED") << std::endl;

  if (argc != 3) {
    printHelp(argv[0]);
    return 1;
  }

  Implementation* fImpl;

  if (std::string(argv[1]) == "naive") {
    fImpl = &primes_naive;
  } else if (std::string(argv[1]) == "optimized"){
    fImpl = &primes_optimized;
  } else if (std::string(argv[1]) == "eratosfen"){
    fImpl = &primes_eratosfen;
  } else {
    printHelp(argv[0]);
    return 1;
  }

  uint32_t N = atoi(argv[2]);
  std::vector<uint32_t> primes;
  primes.reserve(1.1 * N / std::log(N));

  Stopwatch stopwatch;
  stopwatch.start();
  uint32_t nTotal = fImpl(N, primes);
  long time = stopwatch.sinceStartMs();

  std::cout << "Total: " << nTotal << " primes\n";
  std::cout << "Time:  " << time << " ms\n" << std::endl;

  std::cout << "primes: ";
  size_t index = (primes.size() > 25) ? primes.size() - 25 : 0;
  if (index)
    std::cout << " ... ";
  for (; index < primes.size(); ++index)
    std::cout << primes[index] << " ";
  std::cout << std::endl;
  return 0;
}
