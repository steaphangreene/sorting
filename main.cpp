// *************************************************************************
//  This file is part of the Sorting test project by Steaphan Greene
//
//  Copyright 2018 Steaphan Greene <steaphan@gmail.com>
//
//  Sorting is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  Sorting is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Sorting (see the file named "LICENSE");
//  If not, see <http://www.gnu.org/licenses/>.
//
// *************************************************************************

#include <algorithm>
#include <chrono>
#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include <random>
#include <vector>

#include <getopt.h>

#include "data.hpp"
#include "datalist.hpp"

#include "mergesort.hpp"
#include "radixsort.hpp"

unsigned int seed = 123456789;
unsigned long length = (1024 * 1024);

#ifndef DATATYPE
#define DATATYPE uint64_t
#endif
using etype = Data<DATATYPE>;
using dtype = std::vector<etype>;

int main(int argc, char** argv) {
  bool inverted = false;
  bool contiguous = false;
  static option lopts[] = {{"size", 1, 0, 's'}, {0, 0, 0, 0}};
  for (auto arg = '\0'; arg >= 0; arg = getopt_long(argc, argv, "cil:rs:", lopts, nullptr)) {
    if (arg == 'l') {
      length = std::strtoul(optarg, nullptr, 10);
    } else if (arg == 'r') {
      std::random_device rd;
      seed = rd();
    } else if (arg == 's') {
      seed = std::strtol(optarg, nullptr, 10);
    } else if (arg == 'c') {
      contiguous = true;
    } else if (arg == 'i') {
      inverted = true;
    }
  }

  setlocale(LC_NUMERIC, "");
  std::printf("Full run for data length: %'lu\n\n", length);

  auto start = std::chrono::high_resolution_clock::now();
  dtype unsorted;
  unsorted.reserve(length);

  if (contiguous && inverted) {
    for (unsigned long count = length; count > 0; --count) {
      if (std::is_signed<DATATYPE>()) {
        DATATYPE fval = count;
        unsorted.emplace_back(fval - (length / 2 + 1));
      } else {
        unsorted.emplace_back(count);
      }
    }
  } else if (contiguous) {
    for (unsigned long count = 1; count <= length; ++count) {
      if (std::is_signed<DATATYPE>()) {
        DATATYPE fval = count;
        unsorted.emplace_back(fval - (length / 2 + 1));
      } else {
        unsorted.emplace_back(count);
      }
    }
  } else {
    std::mt19937_64 gen(seed);
    for (unsigned long count = 0; count < length; ++count) {
      unsigned long val = gen();
      if (std::is_signed<DATATYPE>() && (count & 1) != 0) {
        DATATYPE fval = val;
        unsorted.emplace_back(-fval);
      } else {
        unsorted.emplace_back(val);
      }
    }
  }

  auto sorted = unsorted;
  sort(sorted.begin(), sorted.end());

  auto finish = std::chrono::high_resolution_clock::now();
  Report("Completing setup", sorted, finish - start);

  Run(sorted, is_sorted, "Built-in std::is_sorted()");

  Run(unsorted, sort, "Built-in std::sort()");
  Run(sorted, sort, "Re-run of built-in std::sort()");

  Run(unsorted, stable_sort, "Built-in std::stable_sort()");
  Run(sorted, stable_sort, "Re-run of built-in std::stable_sort()");

  Run(unsorted, heap_sort, "HeapSort");
  Run(sorted, heap_sort, "Re-run of HeapSort");

  Run(unsorted, MergeSort, "MergeSort");
  Run(sorted, MergeSort, "Re-run of MergeSort");

  RunAs(unsorted, RadixSort, "RadixSort (forward_list)", std::forward_list<etype>);
  RunAs(unsorted, RadixSort, "RadixSort (data_list)", data_list<etype>);

  return 0;
}
