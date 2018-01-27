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

// Stable, Not In-Place
template <typename T>
void StableCountingBinSort(T begin, T end, uint_fast8_t chunk) noexcept {
  std::vector<typename T::value_type> other(begin, end);
  std::array<unsigned long, T::value_type::NumBins()> count = {};

  // Count the items in each bin for the last chunk
  for (auto it = begin; it != end; ++it) {
    ++count[it->GetBin(chunk)];
  }
  // Convert counts to offsets
  unsigned long offset = 0;
  for (unsigned long bin = 0; bin < T::value_type::NumBins(); ++bin) {
    auto new_offset = offset + count[bin];
    count[bin] = offset;
    offset = new_offset;
  }

  // Copy back to the right place in the data array (stable version)
  for (auto it = other.cbegin(); it != other.cend(); ++it) {
    auto bin = it->GetBin(chunk);
    begin[count[bin]++] = *it;
  }

  offset = 0;
  for (unsigned long bin = 0; bin < T::value_type::NumBins(); ++bin) {
    auto next_offset = count[bin];
    using std::is_sorted;
    auto size = next_offset - offset;
    if (!is_sorted(begin + offset, begin + next_offset)) {
      if (size > 1024 && chunk > 0) {
        StableCountingBinSort(begin + offset, begin + next_offset, chunk - 1);
      } else {
        using std::sort;
        sort(begin + offset, begin + next_offset);
      }
    }
    offset = next_offset;
  }
}
template <typename T>
void StableCountingBinSort(T& data) noexcept {
  StableCountingBinSort(data.begin(), data.end(), T::value_type::NumChunks() - 1);
}

// In-Place, Not Stable
template <typename T>
void InPlaceCountingBinSort(T begin, T end, uint_fast8_t chunk) noexcept {
  std::array<unsigned long, T::value_type::NumBins()> count = {};

  // Count the items in each bin for the last chunk
  for (auto it = begin; it != end; ++it) {
    ++count[it->GetBin(chunk)];
  }
  // Convert counts to offsets
  unsigned long offset = 0;
  for (unsigned long bin = 0; bin < T::value_type::NumBins(); ++bin) {
    auto new_offset = offset + count[bin];
    count[bin] = offset;
    offset = new_offset;
  }

  // Swap all elements into their correct palces (non-stable version)
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

  offset = 0;
  for (unsigned long bin = 0; bin < T::value_type::NumBins(); ++bin) {
    auto next_offset = count[bin];
    using std::is_sorted;
    auto size = next_offset - offset;
    if (!is_sorted(begin + offset, begin + next_offset)) {
      if (size > 1024 && chunk > 0) {
        InPlaceCountingBinSort(begin + offset, begin + next_offset, chunk - 1);
      } else {
        using std::sort;
        sort(begin + offset, begin + next_offset);
      }
    }
    offset = next_offset;
  }
}
template <typename T>
void InPlaceCountingBinSort(T& data) noexcept {
  InPlaceCountingBinSort(data.begin(), data.end(), T::value_type::NumChunks() - 1);
}
