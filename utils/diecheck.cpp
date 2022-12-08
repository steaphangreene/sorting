#include <iostream>

int main() {
  uint64_t exp6 = 1, exp2 = 3;
  uint64_t pow6 = 6, pow2 = 8;
  double ratio = double(pow6) / double(pow2);
  double best = ratio;
  std::cerr << ratio << "* : " << pow6 << "(" << exp6 << "):" << pow2 << "(" << exp2 << ")\n";
  while (pow6 < 0xFFFFFFFFFFFFFFFUL) {
    pow6 *= 6;
    exp6 += 1;
    while (pow2 < pow6) {
      pow2 *= 2;
      exp2 += 1;
    }
    ratio = double(pow6) / double(pow2);
    std::cerr << ratio << ((ratio > best) ? "*" : " ") << " : " << pow6 << "(" << exp6
              << "):" << pow2 << "(" << exp2 << ")\n";
    if (ratio > best) {
      best = ratio;
    }
  }

  return 0;
}
