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

#pragma once

#include <mutex>
#include <type_traits>
#include <utility>

#include "color.hpp"

template <typename T>
class Data {
 public:
  Data(const Data<T>& d) noexcept {
    ++stats.constructions;
    ++stats.copies;
    data = d.data;
  }
  Data(Data<T>&& d) noexcept {
    ++stats.constructions;
    ++stats.moves;
    data = std::move(d.data);
  }
  Data(const T& d) noexcept {
    ++stats.constructions;
    ++stats.copies;
    data = d;
  }
  Data(T&& d) noexcept {
    ++stats.constructions;
    ++stats.moves;
    data = std::move(d);
  }
  ~Data() noexcept {
    ++stats.destructions;
  }
  friend void swap(Data<T>& a, Data<T>& b) noexcept {
    ++stats.swaps;
    using std::swap;
    swap(a.data, b.data);
  }
  void operator=(const Data<T>& d) noexcept {
    ++stats.copies;
    data = d.data;
  }
  void operator=(Data<T>&& d) noexcept {
    ++stats.moves;
    data = std::move(d.data);
  }
  bool operator<(const Data<T>& other) const noexcept {
    ++stats.comparisons;
    return data < other.data;
  }
  bool operator==(const Data<T>& other) const noexcept {
    ++stats.comparisons;
    return data == other.data;
  }

  struct Stats {
    void operator=(const Stats& in) = delete;
    void operator=(Stats&& in) = delete;
    Stats(const Stats& in) = delete;
    Stats(Stats&& in) = delete;
    Stats() = default;
    // Though this *is* correct, it would make everything slower.
    // So, you have to manually call GetStats() at the end of every thread.  :/
    // ~Stats() {
    //   Flush();
    // }
    void Flush() noexcept {
      if (this != &(Data<T>::global_stats)) {
        std::lock_guard<std::mutex> lock(stats_lock);
        Data<T>::global_stats.comparisons += comparisons;
        Data<T>::global_stats.copies += copies;
        Data<T>::global_stats.moves += moves;
        Data<T>::global_stats.swaps += swaps;
        Data<T>::global_stats.constructions += constructions;
        Data<T>::global_stats.destructions += destructions;

        Clear();
      }
    }
    void Clear() noexcept {
      comparisons = 0;
      copies = 0;
      moves = 0;
      swaps = 0;
      constructions = 0;
      destructions = 0;
    }
    unsigned long comparisons = 0;
    unsigned long copies = 0;
    unsigned long moves = 0;
    unsigned long swaps = 0;
    unsigned long constructions = 0;
    unsigned long destructions = 0;
  };
  unsigned long GetBin(uint_fast8_t chunk) const noexcept {
    ++stats.comparisons;
    union {
      T base_type;
      uint16_t binary[NumChunks()];
    } converter;
    converter.base_type = data;
    auto bin = converter.binary[chunk];
    if (std::is_floating_point<T>()) {
      if (chunk == NumChunks() - 1) {
        if (bin & 0x8000) { // Negative
          bin = 0x10000 - bin;
        } else { // Positive
          bin = 0x8000 + bin;
        }
      } else {
        if (converter.binary[NumChunks() - 1] & 0x8000) { // Negative
          bin ^= 0xFFFF;
        }
      }
    } else if (std::is_signed<T>()) {
      if (chunk == NumChunks() - 1) {
        bin ^= 0x8000; // Proper order for MSW of 2's comp encoded number
      }
    }
    return bin;
  }
  constexpr static unsigned long NumBins() noexcept {
    return 0x10000UL;
  }
  constexpr static uint_fast8_t NumChunks() noexcept {
    constexpr auto ret = (sizeof(T) + 1) / 2;
    if (std::is_floating_point<T>()) {
      if (ret > 5) {
        return 5; // Unpad long double (WARNING: Hard-Coded)
      }
    }
    return ret;
  }

  static Stats& GetStats() noexcept {
    stats.Flush();
    return global_stats;
  }
  static unsigned long GetComparisons() noexcept {
    return stats.comparisons;
  }
  static unsigned long GetCopies() noexcept {
    return stats.copies;
  }
  static unsigned long GetMoves() noexcept {
    return stats.moves;
  }
  static unsigned long GetSwaps() noexcept {
    return stats.swaps;
  }
  static unsigned long GetConstructions() noexcept {
    return stats.constructions;
  }
  static unsigned long GetDestructions() noexcept {
    return stats.destructions;
  }
  static void ResetStats() noexcept {
    stats.Clear();
    global_stats.Clear();
  }

 private:
  T data;
  static thread_local Stats stats;
  static Stats global_stats;
  static std::mutex stats_lock;
};

template <typename T>
thread_local typename Data<T>::Stats Data<T>::stats;

template <typename T>
typename Data<T>::Stats Data<T>::global_stats;

template <typename T>
typename std::mutex Data<T>::stats_lock;

template <typename T, typename V>
void Report(const char* mes, T& data, V took) {
  auto& stats = T::value_type::GetStats();
  printf("Took " CRED "%'15ldns" CNRM " for: " CGRN "%s" CNRM "\n", took.count(), mes);
  printf("  " CBLU "%'15lu" CNRM " Comp ", stats.comparisons);
  printf("  " CBLU "%'15lu" CNRM " Cnst ", stats.constructions);
  printf("  " CBLU "%'15lu" CNRM " Dstr \n", stats.destructions);
  printf("  " CBLU "%'15lu" CNRM " Swap ", stats.swaps);
  printf("  " CBLU "%'15lu" CNRM " Copy ", stats.copies);
  printf("  " CBLU "%'15lu" CNRM " Move \n", stats.moves);
  T::value_type::ResetStats();
  using std::is_sorted;
  if (!is_sorted(data.cbegin(), data.cend())) {
    printf("  Warning: Data is not correctly sorted!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  }
  printf("\n");
}

template <typename T>
auto is_sorted(T& in_data) noexcept {
  return std::is_sorted(in_data.begin(), in_data.end());
}

template <typename T>
void sort(T& in_data) noexcept {
  std::sort(in_data.begin(), in_data.end());
}

template <typename T>
void heap_sort(T& in_data) noexcept {
  std::make_heap(in_data.begin(), in_data.end());
  std::sort_heap(in_data.begin(), in_data.end());
}

template <typename T>
void stable_sort(T& in_data) noexcept {
  std::stable_sort(in_data.begin(), in_data.end());
}

#define RunCommon(in_data, func, mes, sdtype)                \
  {                                                          \
    etype::ResetStats();                                     \
    auto start = std::chrono::high_resolution_clock::now();  \
    func(in_data);                                           \
    auto finish = std::chrono::high_resolution_clock::now(); \
    Report(mes, in_data, finish - start);                    \
  }
#define Run(from_data, func, mes)          \
  {                                        \
    auto to_data = from_data;              \
    RunCommon(to_data, func, mes, sdtype); \
  }
#define RunAs(from_data, func, mes, sdtype)             \
  {                                                     \
    sdtype to_data(from_data.begin(), from_data.end()); \
    RunCommon(to_data, func, mes, sdtype);              \
  }
