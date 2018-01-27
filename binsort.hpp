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
void BinSort(T& data) noexcept {
  constexpr unsigned long num_bins = T::value_type::NumBins();
  T bins[num_bins];

  // Splice data into bins
  while (!data.empty()) {
    auto it = data.cbefore_begin();
    auto bin = next(it)->GetBin(it->NumChunks() - 1);
    bins[bin].splice_after(bins[bin].cbefore_begin(), data, it);
  }

  // Sort each bin, and then splice sorted data back out of bins
  auto place = data.cbefore_begin();
  for (unsigned long bin = 0; bin < num_bins; ++bin) {
    if (!is_sorted(bins[bin].cbegin(), bins[bin].cend())) {
      bins[bin].sort();
    }
    while (!bins[bin].empty()) {
      data.splice_after(place, bins[bin], bins[bin].cbefore_begin());
      ++place;
    }
  }
}
