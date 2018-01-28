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

#include <memory>

// This data_list class is a minimal implementation of a singly-linked list
// for temporary us by sorting algorithms that require such a structure.

template <typename T>
struct data_node {
  data_node* next;
  T value;
};

template <typename T>
class data_list {
 public:
  data_list() = default;

  template <typename Iter>
  data_list(Iter b, Iter e) {
    auto in_size = e - b;
    if (in_size > 0) {
      memory = static_cast<data_node<T>*>(std::malloc(sizeof(data_node<T>) * in_size));
      size = 0;
      for (auto it = b; it != e; ++it) {
        memory[size].next = memory + size + 1;
        memory[size].value = *it;
        ++size;
      }
      head.next = memory;
      memory[size - 1].next = nullptr;
    }
  }
  ~data_list() {
    if (memory != nullptr)
      free(memory);
  }

  bool empty() {
    return head.next == nullptr;
  }

  using value_type = T;

  class iterator {
   public:
    friend class data_list;
    iterator(data_node<T>* in_node) {
      node = in_node;
    };
    void operator++() {
      node = node->next;
    }
    T* operator->() const {
      return &node->value;
    }
    T& operator*() {
      return node->value;
    }
    bool operator==(const typename data_list<T>::iterator& other) const {
      return node == other.node;
    }
    bool operator!=(const typename data_list<T>::iterator& other) const {
      return node != other.node;
    }
    friend data_list<T>::iterator next(data_list<T>::iterator itr) {
      return data_list<T>::iterator(itr.node->next);
    }
    friend bool is_sorted(const data_list<T>::iterator& b, const data_list<T>::iterator& e) {
      if (b == e)
        return true;
      auto itr = b;
      while (next(itr) != e) {
        if (next(itr).node->value < itr.node->value) {
          return false;
        }
        ++itr;
      }
      return true;
    }

   private:
    data_node<T>* node;
  };
  data_list<T>::iterator before_begin() {
    return data_list<T>::iterator(&head);
  }
  data_list<T>::iterator begin() {
    return data_list<T>::iterator(head.next);
  }
  data_list<T>::iterator end() {
    return data_list<T>::iterator(nullptr);
  }
  const data_list<T>::iterator cbefore_begin() {
    return data_list<T>::iterator(&head);
  }
  const data_list<T>::iterator cbegin() {
    return data_list<T>::iterator(head.next);
  }
  const data_list<T>::iterator cend() {
    return data_list<T>::iterator(nullptr);
  }

  void sort() {
    quick_sort(&head, static_cast<data_node<T>*>(nullptr));
  }
  void splice_after(data_list<T>::iterator to, data_list<T>&, data_list<T>::iterator from) {
    auto mover = from.node->next;
    from.node->next = from.node->next->next;
    mover->next = to.node->next;
    to.node->next = mover;
  }

 private:
  data_node<T> head = {nullptr, 0};
  data_node<T>* memory = nullptr;
  unsigned long size = 0;
  data_node<T>* partition(data_node<T>* begin, data_node<T>* end) {
    auto less = begin;
    auto pivot = begin->next;
    auto more = pivot;
    while (more->next != end) {
      if (more->next->value < pivot->value) {
        splice_after(less, *this, more);
      } else {
        more = more->next;
      }
    }
    return pivot;
  }
  void quick_sort(data_node<T>* begin, data_node<T>* end) {
    if (begin != nullptr && begin->next != end) {
      auto mid = partition(begin, end);
      quick_sort(begin, mid);
      quick_sort(mid, end);
    }
  }
};
