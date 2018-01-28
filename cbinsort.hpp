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

#include <future>

template <typename T, typename A>
void CountingBinSubSort(
    T begin,
    T end,
    unsigned long offset,
    const A& count,
    bool in_place,
    uint_fast8_t chunk) noexcept {
  for (unsigned long bin = 0; bin < count.size(); ++bin) {
    auto next_offset = count[bin];
    using std::is_sorted;
    auto size = next_offset - offset;
    if (!is_sorted(begin + offset, begin + next_offset)) {
      if (size > 1024 && chunk > 0) {
        CountingBinSort(begin + offset, begin + next_offset, in_place, 1, chunk - 1);
      } else {
        using std::sort;
        sort(begin + offset, begin + next_offset);
      }
    }
    offset = next_offset;
  }
}

template <typename T>
void CountingBinSort(T begin, T end, bool in_place, int threads, uint_fast8_t chunk) noexcept {
  std::array<unsigned long, T::value_type::NumBins()> count = {};

  // Count the items in each bin for the last chunk
  for (auto it = begin; it != end; ++it) {
    ++count[it->GetBin(chunk)];
  }
  // Convert counts to offsets
  unsigned long offset = 0;
  for (unsigned long bin = 0; bin < T::value_type::NumBins(); ++bin) {
    auto next_offset = offset + count[bin];
    count[bin] = offset;
    offset = next_offset;
  }

  if (in_place) {
    // Swap all elements into their correct palces (in-place, non-stable version)
    auto place = count;
    for (auto it = begin; it != end; ++it) {
      if (count[it->GetBin(chunk)] != static_cast<unsigned long>(it - begin)) {
        auto temp = *it;
        offset = static_cast<unsigned long>(1 + it - begin);
        while (offset != static_cast<unsigned long>(it - begin)) {
          auto bin = temp.GetBin(chunk);
          offset = place[bin]++;
          using std::swap;
          swap(temp, begin[offset]);
        }
      }
      ++count[it->GetBin(chunk)];
    }
  } else {
    // Copy list, then copy back to the right place in the data array (stable version)
    std::vector<typename T::value_type> other(begin, end);
    for (auto it = other.cbegin(); it != other.cend(); ++it) {
      auto bin = it->GetBin(chunk);
      begin[count[bin]++] = *it;
    }
  }

  if (threads < 2) {
    CountingBinSubSort(begin, end, 0, count, in_place, chunk);
  } else {
    std::vector<std::future<void>> futures;

    auto step = T::value_type::NumBins() / threads;
    unsigned long offset = 0;
    for (auto thread = 0; thread < threads; ++thread) {
      unsigned long next_offset = count[step * (thread + 1) - 1];
      std::vector<unsigned long> cnt(
          count.begin() + step * thread, count.begin() + step * (thread + 1));

      futures.push_back(
          std::async(std::launch::async, [begin, end, offset, cnt, in_place, chunk]() {
            CountingBinSubSort(begin, end, offset, cnt, in_place, chunk);
            begin->GetStats(); // Must manually force a stats flush before this thread goes away.
          }));

      offset = next_offset;
    }
    for (auto& fut : futures) {
      fut.get();
    }
    futures.clear();
  }
}

// Stable, Not In-Place
template <typename T>
void StableCountingBinSort(T& data) noexcept {
  CountingBinSort(data.begin(), data.end(), false, 1, T::value_type::NumChunks() - 1);
}

// In-Place, Not Stable
template <typename T>
void InPlaceCountingBinSort(T& data) noexcept {
  CountingBinSort(data.begin(), data.end(), true, 1, T::value_type::NumChunks() - 1);
}

// Stable, Not In-Place, 2 Threads
template <typename T>
void DualStableCountingBinSort(T& data) noexcept {
  CountingBinSort(data.begin(), data.end(), false, 2, T::value_type::NumChunks() - 1);
}

// In-Place, Not Stable, 2 Threads
template <typename T>
void DualInPlaceCountingBinSort(T& data) noexcept {
  CountingBinSort(data.begin(), data.end(), true, 2, T::value_type::NumChunks() - 1);
}

// Stable, Not In-Place, 4 Threads
template <typename T>
void QuadStableCountingBinSort(T& data) noexcept {
  CountingBinSort(data.begin(), data.end(), false, 4, T::value_type::NumChunks() - 1);
}

// In-Place, Not Stable, 4 Threads
template <typename T>
void QuadInPlaceCountingBinSort(T& data) noexcept {
  CountingBinSort(data.begin(), data.end(), true, 4, T::value_type::NumChunks() - 1);
}

// Stable, Not In-Place, 8 Threads
template <typename T>
void OctoStableCountingBinSort(T& data) noexcept {
  CountingBinSort(data.begin(), data.end(), false, 8, T::value_type::NumChunks() - 1);
}

// In-Place, Not Stable, 8 Threads
template <typename T>
void OctoInPlaceCountingBinSort(T& data) noexcept {
  CountingBinSort(data.begin(), data.end(), true, 8, T::value_type::NumChunks() - 1);
}
