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
#include <future>
#include <vector>

template <typename T>
struct SplitSortFutureBox {
  static thread_local std::vector<std::future<void>> futures;
};
template <typename T>
thread_local std::vector<std::future<void>> SplitSortFutureBox<T>::futures;

template <typename Ti>
auto do_partition(Ti first, Ti last) noexcept {
  if (last - first < 2)
    return first;
  const auto pivot = *(first + ((last - first) / 2));
  return std::partition(first, last, [pivot](const auto& em) { return em < pivot; });
}

template <typename T>
void WaitAll(T&) noexcept {
  SplitSortFutureBox<typename T::value_type> future_box;
  auto& futures = future_box.futures;

  for (auto& fut : futures) {
    fut.get();
  }
  futures.clear();
}

template <typename Ti>
void UniSort(Ti first, Ti last, bool wait = true) noexcept {
  SplitSortFutureBox<typename Ti::value_type> future_box;
  auto& futures = future_box.futures;

  if ((last - first) < 1024) {
    // Fewer than 1k items: Below sanity limit - don't bother to thread.
    sort(first, last);
  } else {
    futures.push_back(std::async(std::launch::async, [first, last]() {
      sort(first, last);
      first->GetStats(); // Have to manually force a stats flush before this thread goes away.
    }));
    if (wait)
      WaitAll(first);
  }
}
template <typename T>
void UniSort(T& data) noexcept {
  UniSort(data.begin(), data.end());
}

template <typename Ti>
void DualSort(Ti first, Ti last, bool wait = true) noexcept {
  auto mid = do_partition(first, last);
  UniSort(first, mid, false);
  UniSort(mid, last, false);
  if (wait)
    WaitAll(first);
}
template <typename T>
void DualSort(T& data) noexcept {
  DualSort(data.begin(), data.end());
}

template <typename Ti>
void QuadSort(Ti first, Ti last, bool wait = true) noexcept {
  auto mid = do_partition(first, last);
  DualSort(first, mid, false);
  DualSort(mid, last, false);
  if (wait)
    WaitAll(first);
}
template <typename T>
void QuadSort(T& data) noexcept {
  QuadSort(data.begin(), data.end());
}

template <typename Ti>
void OctoSort(Ti first, Ti last, bool wait = true) noexcept {
  auto mid = do_partition(first, last);
  QuadSort(first, mid, false);
  QuadSort(mid, last, false);
  if (wait)
    WaitAll(first);
}
template <typename T>
void OctoSort(T& data) noexcept {
  OctoSort(data.begin(), data.end());
}
