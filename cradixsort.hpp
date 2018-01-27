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

template <typename T>
void CountingRadixSort(T& data) noexcept {
  auto other = data;
  unsigned long count[T::value_type::NumChunks()][T::value_type::NumBins()] = {};

  // Count the items for each bin
  for (auto it = other.cbegin(); it != other.cend(); ++it) {
    for (uint_fast8_t chunk = 0; chunk < T::value_type::NumChunks(); ++chunk) {
      ++count[chunk][it->GetBin(chunk)];
    }
  }
  // Convert counts to offsets
  for (uint_fast8_t chunk = 0; chunk < T::value_type::NumChunks(); ++chunk) {
    unsigned long offset = 0;
    for (unsigned long bin = 0; bin < T::value_type::NumBins(); ++bin) {
      auto new_offset = offset + count[chunk][bin];
      count[chunk][bin] = offset;
      offset = new_offset;
    }
  }

  // Copy to the right place in the other array
  if (T::value_type::NumChunks() > 3) {
    auto chunk = T::value_type::NumChunks() - 4;
    for (auto it = data.cbegin(); it != data.cend(); ++it) {
      auto bin = it->GetBin(chunk);
      other[count[chunk][bin]++] = *it;
    }
  }

  // Copy to the right place in the other array
  if (T::value_type::NumChunks() > 2) {
    auto chunk = T::value_type::NumChunks() - 3;
    for (auto it = other.cbegin(); it != other.cend(); ++it) {
      auto bin = it->GetBin(chunk);
      data[count[chunk][bin]++] = *it;
    }
  }

  // Copy to the right place in the other array
  if (T::value_type::NumChunks() > 1) {
    auto chunk = T::value_type::NumChunks() - 2;
    for (auto it = data.cbegin(); it != data.cend(); ++it) {
      auto bin = it->GetBin(chunk);
      other[count[chunk][bin]++] = *it;
    }
  }

  // Copy back to the right place in the data array
  auto chunk = T::value_type::NumChunks() - 1;
  for (auto it = other.cbegin(); it != other.cend(); ++it) {
    auto bin = it->GetBin(chunk);
    data[count[chunk][bin]++] = *it;
  }
}
