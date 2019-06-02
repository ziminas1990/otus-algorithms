#include <iostream>
#include <stdint.h>
#include <string>
#include <math.h>
#include "Stopwatch.h"

using Implementation = double(uint32_t N);

double fibonacci_naive(uint32_t N)
{
  if (N <= 2)
    return 1;
  return fibonacci_naive(N - 2) + fibonacci_naive(N - 1);
}

double fibonacci_iterative(uint32_t N)
{
  if (N < 3)
    return 1;

  uint64_t a = 1;
  uint64_t b = 1;
  double r = 0;
  for(size_t i = 3; i <= N; ++i) {
    r = a + b;
    a = b;
    b = r;
  }
  return r;
}

double fibonacci_gold(uint32_t N)
{
  double sqrt5 = sqrt(5);
  double k = (1 + sqrt5) / 2;
  return pow(k, N) / sqrt5 + 0.5;
}

void printHelp(std::string const& selfApp)
{
  std::cout << "Help:\n";
  std::cout << "    " << selfApp << " <string: recursive/iterative/gold> <long: N>\n";
  std::cout << "Examples:\n";
  std::cout << "    " << selfApp << " recursive 1000\n";
  std::cout << "    " << selfApp << " iterative 1000\n";
  std::cout << "    " << selfApp << " gold      1000" << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc != 3) {
    printHelp(argv[0]);
    return 1;
  }

  Implementation* fImpl;

  if (std::string(argv[1]) == "recursive") {
    fImpl = &fibonacci_naive;
  } else if (std::string(argv[1]) == "iterative"){
    fImpl = &fibonacci_iterative;
  } else if (std::string(argv[1]) == "gold"){
    fImpl = &fibonacci_gold;
  } else {
    printHelp(argv[0]);
    return 1;
  }

  uint32_t N = atoi(argv[2]);

  Stopwatch stopwatch;
  stopwatch.start();
  double nResult = fImpl(N);
  long time = stopwatch.sinceStartMs();

  std::cout << "Result: " << nResult << "\n";
  std::cout << "Time:   " << time << " ms\n" << std::endl;
  return 0;
}
