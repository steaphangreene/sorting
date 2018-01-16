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
void RadixSort(T& data) noexcept {
  constexpr unsigned long num_bins = T::value_type::NumBins();

  T table[2][num_bins];

  // Splice data into table[0] (or table[1] for odd-word types), radix step 1
  uint_fast8_t chunk = 0;
  while (!data.empty()) {
    auto to = data.cbefore_begin()->NumChunks() & 1;
    auto it = data.cbefore_begin();
    auto bin = next(it)->GetBin(chunk);
    table[to][bin].splice_after(table[to][bin].cbefore_begin(), data, it);
  }

  // Splice for steps 2 through N within the table
  ++chunk;
  for (; chunk < data.cbefore_begin()->NumChunks(); ++chunk) {
    auto to = chunk & 1;
    auto from = to ^ 1;

    // Splice even step (or odd for odd-word types) into table[1]
    if ((data.cbefore_begin()->NumChunks() & 1) == 0 || chunk > 1) {
      for (unsigned long frombin = num_bins; frombin > 0;) {
        --frombin;
        while (!table[from][frombin].empty()) {
          auto it = table[from][frombin].cbefore_begin();
          auto tobin = next(it)->GetBin(chunk);
          table[to][tobin].splice_after(table[to][tobin].cbefore_begin(), table[from][frombin], it);
        }
      }
      ++chunk;
    }

    // Splice odd step (or even for odd-word types) into table[0]
    if (chunk < data.cbefore_begin()->NumChunks()) {
      std::swap(to, from);
      for (unsigned long frombin = 0; frombin < num_bins; ++frombin) {
        while (!table[from][frombin].empty()) {
          auto it = table[from][frombin].cbefore_begin();
          auto tobin = next(it)->GetBin(chunk);
          table[to][tobin].splice_after(table[to][tobin].cbefore_begin(), table[from][frombin], it);
        }
      }
    }
  }

  // Splice sorted data back out of table[0]
  auto from = (chunk & 1) ^ 1;
  auto place = data.cbefore_begin();
  for (unsigned long bin = 0; bin < num_bins; ++bin) {
    while (!table[from][bin].empty()) {
      data.splice_after(place, table[from][bin], table[from][bin].cbefore_begin());
      ++place;
    }
  }
}
