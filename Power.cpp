#include <iostream>
#include <stdint.h>
#include <string>
#include "Stopwatch.h"

using Implementation = double(double base, uint64_t power);

double power_naive(double base, uint64_t power)
{
  double result = 1;
  for(size_t i = 0; i < power; ++i)
    result *= base;
  return result;
}

double power_optimized(double base, uint64_t power)
{
  if (!power)
    return 1;
  double result          = base;
  uint64_t nHalfPower    = power >> 1;
  uint64_t nCurrentPower = 1;
  while (nCurrentPower < power) {
    if (nCurrentPower > nHalfPower) {
      result *= base;
      ++nCurrentPower;
    } else {
      result *= result;
      nCurrentPower <<= 1;
    }
  }
  return result;
}

double power_best(double base, uint64_t power)
{
  if (!power)
    return 1;
  double result = 1;
  while (power > 1) {
    if (power % 2)
      result *= base;
    base *= base;
    power >>= 1;
  }
  return result * base;
}

void printHelp(std::string const& selfApp)
{
  std::cout << "Help:\n";
  std::cout << "    " << selfApp << " <string: naive/optimized/best> <double: base> <long: power>\n";
  std::cout << "Examples:\n";
  std::cout << "    " << selfApp << " naive     1.00000001   500000000\n";
  std::cout << "    " << selfApp << " optimized 1.000000001  5000000000\n";
  std::cout << "    " << selfApp << " best      1.0000000001 50000000000" << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc != 4) {
    printHelp(argv[0]);
    return 1;
  }

  Implementation* fImpl;

  if (std::string(argv[1]) == "naive") {
    fImpl = &power_naive;
  } else if (std::string(argv[1]) == "optimized"){
    fImpl = &power_optimized;
  } else if (std::string(argv[1]) == "best"){
    fImpl = &power_best;
  } else {
    printHelp(argv[0]);
    return 1;
  }

  double   a = atof(argv[2]);
  uint64_t b = atoll(argv[3]);

  Stopwatch stopwatch;
  stopwatch.start();
  double result = fImpl(a, b);
  long time = stopwatch.sinceStartMs();

  std::cout << "Result: " << result << std::endl;
  std::cout << "Time:   " << time << " ms" << std::endl;
  return 0;
}
