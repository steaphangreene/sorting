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

template <typename Ti>
void MergeSort(Ti first, Ti last, bool wait = true) noexcept {
  if (last - first > 1) {
    Ti middle = first + (last - first) / 2;
    MergeSort(first, middle);
    MergeSort(middle, last);
    std::inplace_merge(first, middle, last);
  }
}
template <typename T>
void MergeSort(T& data, bool wait = true) noexcept {
  MergeSort(data.begin(), data.end(), wait);
}
