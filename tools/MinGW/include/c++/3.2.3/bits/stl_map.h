// Map implementation -*- C++ -*-

// Copyright (C) 2001, 2002 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file stl_map.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _CPP_BITS_STL_MAP_H
#define _CPP_BITS_STL_MAP_H 1

#include <bits/concept_check.h>

namespace std
{

/**
 *  @brief A standard container made up of pairs (see std::pair in <utility>)
 *         which can be retrieved based on a key.
 *
 *  This is an associative container.  Values contained within it can be
 *  quickly retrieved through a key element.  Example:  MyMap["First"] would
 *  return the data associated with the key "First".
*/
template <class _Key, class _Tp, class _Compare = less<_Key>,
          class _Alloc = allocator<pair<const _Key, _Tp> > >
class map
{
  // concept requirements
  __glibcpp_class_requires(_Tp, _SGIAssignableConcept)
  __glibcpp_class_requires4(_Compare, bool, _Key, _Key, _BinaryFunctionConcept);

public:
  // typedefs:
  typedef _Key                 key_type;
  typedef _Tp                   data_type;
  typedef _Tp                   mapped_type;
  typedef pair<const _Key, _Tp> value_type;
  typedef _Compare             key_compare;

  class value_compare
    : public binary_function<value_type, value_type, bool> {
  friend class map<_Key,_Tp,_Compare,_Alloc>;
  protected :
    _Compare comp;
    value_compare(_Compare __c) : comp(__c) {}
  public:
    bool operator()(const value_type& __x, const value_type& __y) const {
      return comp(__x.first, __y.first);
    }
  };

private:
  typedef _Rb_tree<key_type, value_type,
                   _Select1st<value_type>, key_compare, _Alloc> _Rep_type;
  _Rep_type _M_t;  // red-black tree representing map
public:
  typedef typename _Rep_type::pointer pointer;
  typedef typename _Rep_type::const_pointer const_pointer;
  typedef typename _Rep_type::reference reference;
  typedef typename _Rep_type::const_reference const_reference;
  typedef typename _Rep_type::iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef typename _Rep_type::reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::allocator_type allocator_type;

  // allocation/deallocation

  map() : _M_t(_Compare(), allocator_type()) {}
  explicit map(const _Compare& __comp,
               const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) {}

  template <class _InputIterator>
  map(_InputIterator __first, _InputIterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_unique(__first, __last); }

  template <class _InputIterator>
  map(_InputIterator __first, _InputIterator __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }
  map(const map<_Key,_Tp,_Compare,_Alloc>& __x) : _M_t(__x._M_t) {}

  map<_Key,_Tp,_Compare,_Alloc>&
  operator=(const map<_Key, _Tp, _Compare, _Alloc>& __x)
  {
    _M_t = __x._M_t;
    return *this;
  }

  // accessors:

  key_compare key_comp() const { return _M_t.key_comp(); }
  value_compare value_comp() const { return value_compare(_M_t.key_comp()); }
  allocator_type get_allocator() const { return _M_t.get_allocator(); }

  /**
   *  Returns a read/write iterator that points to the first pair in the map.
   *  Iteration is done in ascending order according to the keys.
  */
  iterator begin() { return _M_t.begin(); }

  /**
   *  Returns a read-only (constant) iterator that points to the first pair
   *  in the map.  Iteration is done in ascending order according to the keys.
  */
  const_iterator begin() const { return _M_t.begin(); }

  /**
   *  Returns a read/write iterator that points one past the last pair in the
   *  map.  Iteration is done in ascending order according to the keys.
  */
  iterator end() { return _M_t.end(); }

  /**
   *  Returns a read-only (constant) iterator that points one past the last
   *  pair in the map.  Iteration is done in ascending order according to the
   *  keys.
  */
  const_iterator end() const { return _M_t.end(); }

  /**
   *  Returns a read/write reverse iterator that points to the last pair in
   *  the map.  Iteration is done in descending order according to the keys.
  */
  reverse_iterator rbegin() { return _M_t.rbegin(); }

  /**
   *  Returns a read-only (constant) reverse iterator that points to the last
   *  pair in the map.  Iteration is done in descending order according to
   *  the keys.
  */
  const_reverse_iterator rbegin() const { return _M_t.rbegin(); }

  /**
   *  Returns a read/write reverse iterator that points to one before the
   *  first pair in the map.  Iteration is done in descending order according
   *  to the keys.
  */
  reverse_iterator rend() { return _M_t.rend(); }

  /**
   *  Returns a read-only (constant) reverse iterator that points to one
   *  before the first pair in the map.  Iteration is done in descending order
   *  according to the keys.
  */
  const_reverse_iterator rend() const { return _M_t.rend(); }

  /** Returns true if the map is empty.  (Thus begin() would equal end().)  */
  bool empty() const { return _M_t.empty(); }
  /** Returns the size of the map.  */
  size_type size() const { return _M_t.size(); }
  /** Returns the maximum size of the map.  */
  size_type max_size() const { return _M_t.max_size(); }

  /**
   *  @brief Subscript ( [] ) access to map data.
   *  @param  k  The key for which data should be retrieved.
   * 
   *  Allows for easy lookup with the subscript ( [] ) operator.  Returns the
   *  data associated with the key specified in subscript.  If the key does
   *  not exist a pair with that key is created with a default value, which
   *  is then returned.
  */
  _Tp& operator[](const key_type& __k) {
    iterator __i = lower_bound(__k);
    // __i->first is greater than or equivalent to __k.
    if (__i == end() || key_comp()(__k, (*__i).first))
      __i = insert(__i, value_type(__k, _Tp()));
    return (*__i).second;
  }

  void swap(map<_Key,_Tp,_Compare,_Alloc>& __x) { _M_t.swap(__x._M_t); }

  // insert/erase
  /**
   *  @brief Attempts to insert a std::pair into the map.
   *  @param  x  Pair to be inserted (see std::make_pair for easy creation of
   *             pairs).
   *  @return  A pair of which the first element is an iterator that points
   *           to the possibly inserted pair, a second element of type bool
   *           to show if the pair was actually inserted.
   *
   *  This function attempts to insert a (key, value) pair into the map.  A
   *  map relies on unique keys and thus a pair is only inserted if its first
   *  element (the key) is not already present in the map.
  */
  pair<iterator,bool> insert(const value_type& __x)
    { return _M_t.insert_unique(__x); }

  /**
   *  @brief Attempts to insert a std::pair into the map.
   *  @param  position  An iterator that serves as a hint as to where the
   *                    pair should be inserted.
   *  @param  x  Pair to be inserted (see std::make_pair for easy creation of
   *             pairs).
   *  @return  An iterator that points to the inserted (key,value) pair.
   *
   *  This function is not concerned about whether the insertion took place
   *  or not and thus does not return a boolean like the single-argument
   *  insert() does.  Note that the first parameter is only a hint and can
   *  potentially improve the performance of the insertion process.  A bad
   *  hint would cause no gains in efficiency.
  */
  iterator insert(iterator position, const value_type& __x)
    { return _M_t.insert_unique(position, __x); }

  /**
   *  @brief A template function that attemps to insert elements from
   *         another range (possibly another map).
   *  @param  first  Iterator pointing to the start of the range to be inserted.
   *  @param  last  Iterator pointing to the end of the range.
  */
  template <class _InputIterator>
  void insert(_InputIterator __first, _InputIterator __last) {
    _M_t.insert_unique(__first, __last);
  }

  /**
   *  @brief Erases an element from a map.
   *  @param  position  An iterator pointing to the element to be erased.
   *
   *  This function erases an element, pointed to by the given iterator, from
   *  a map.  Note that this function only erases the element, and that if
   *  the element is itself a pointer, the pointed-to memory is not touched
   *  in any way.  Managing the pointer is the user's responsibilty.
  */
  void erase(iterator __position) { _M_t.erase(__position); }

  /**
   *  @brief Erases an element according to the provided key.
   *  @param  x  Key of element to be erased.
   *  @return  Doc me! (Number of elements that match key? Only makes sense
   *           with multimap)
   *
   *  This function erases an element, located by the given key, from a map.
   *  Note that this function only erases the element, and that if
   *  the element is itself a pointer, the pointed-to memory is not touched
   *  in any way.  Managing the pointer is the user's responsibilty.
  */
  size_type erase(const key_type& __x) { return _M_t.erase(__x); }

  /**
   *  @brief Erases a [first,last) range of elements from a map.
   *  @param  first  Iterator pointing to the start of the range to be erased.
   *  @param  last  Iterator pointing to the end of the range to be erased.
   *
   *  This function erases a sequence of elements from a map.
   *  Note that this function only erases the element, and that if
   *  the element is itself a pointer, the pointed-to memory is not touched
   *  in any way.  Managing the pointer is the user's responsibilty.
  */
  void erase(iterator __first, iterator __last)
    { _M_t.erase(__first, __last); }

  /** Erases all elements in a map.  Note that this function only erases
   *  the elements, and that if the elements themselves are pointers, the
   *  pointed-to memory is not touched in any way.  Managing the pointer is
   *  the user's responsibilty.
  */
  void clear() { _M_t.clear(); }

  // map operations:

  /**
   *  @brief Tries to locate an element in a map.
   *  @param  x  Key of (key, value) pair to be located.
   *  @return  Iterator pointing to sought-after element, or end() if not
   *           found.
   *
   *  This function takes a key and tries to locate the element with which
   *  the key matches.  If successful the function returns an iterator
   *  pointing to the sought after pair. If unsuccessful it returns the
   *  one past the end ( end() ) iterator.
  */
  iterator find(const key_type& __x) { return _M_t.find(__x); }

  /**
   *  @brief Tries to locate an element in a map.
   *  @param  x  Key of (key, value) pair to be located.
   *  @return  Read-only (constant) iterator pointing to sought-after
   *           element, or end() if not found.
   *
   *  This function takes a key and tries to locate the element with which
   *  the key matches.  If successful the function returns a constant iterator
   *  pointing to the sought after pair. If unsuccessful it returns the
   *  one past the end ( end() ) iterator.
  */
  const_iterator find(const key_type& __x) const { return _M_t.find(__x); }

  /**
   *  @brief Finds the number of elements with given key.
   *  @param  x  Key of (key, value) pairs to be located.
   *  @return Number of elements with specified key.
   *
   *  This function only makes sense for multimaps.
  */
  size_type count(const key_type& __x) const {
    return _M_t.find(__x) == _M_t.end() ? 0 : 1;
  }

  /**
   *  @brief Finds the beginning of a subsequence matching given key.
   *  @param  x  Key of (key, value) pair to be located.
   *  @return  Iterator pointing to first element matching given key, or
   *           end() if not found.
   *
   *  This function is useful only with std::multimap.  It returns the first
   *  element of a subsequence of elements that matches the given key.  If
   *  unsuccessful it returns an iterator pointing to the first element that
   *  has a greater value than given key or end() if no such element exists.
  */
  iterator lower_bound(const key_type& __x) {return _M_t.lower_bound(__x); }

  /**
   *  @brief Finds the beginning of a subsequence matching given key.
   *  @param  x  Key of (key, value) pair to be located.
   *  @return  Read-only (constant) iterator pointing to first element
   *           matching given key, or end() if not found.
   *
   *  This function is useful only with std::multimap.  It returns the first
   *  element of a subsequence of elements that matches the given key.  If
   *  unsuccessful the iterator will point to the next greatest element or,
   *  if no such greater element exists, to end().
  */
  const_iterator lower_bound(const key_type& __x) const {
    return _M_t.lower_bound(__x);
  }

  /**
   *  @brief Finds the end of a subsequence matching given key.
   *  @param  x  Key of (key, value) pair to be located.
   *  @return Iterator pointing to last element matching given key.
   *
   *  This function only makes sense with multimaps.
  */
  iterator upper_bound(const key_type& __x) {return _M_t.upper_bound(__x); }

  /**
   *  @brief Finds the end of a subsequence matching given key.
   *  @param  x  Key of (key, value) pair to be located.
   *  @return  Read-only (constant) iterator pointing to last element matching
   *           given key.
   *
   *  This function only makes sense with multimaps.
  */
  const_iterator upper_bound(const key_type& __x) const {
    return _M_t.upper_bound(__x);
  }

  /**
   *  @brief Finds a subsequence matching given key.
   *  @param  x  Key of (key, value) pairs to be located.
   *  @return  Pair of iterators that possibly points to the subsequence
   *           matching given key.
   *
   *  This function improves on lower_bound() and upper_bound() by giving a more
   *  elegant and efficient solution.  It returns a pair of which the first
   *  element possibly points to the first element matching the given key
   *  and the second element possibly points to the last element matching the
   *  given key.  If unsuccessful the first element of the returned pair will
   *  contain an iterator pointing to the next greatest element or, if no such
   *  greater element exists, to end().
   *
   *  This function only makes sense for multimaps.
  */
  pair<iterator,iterator> equal_range(const key_type& __x) {
    return _M_t.equal_range(__x);
  }

  /**
   *  @brief Finds a subsequence matching given key.
   *  @param  x  Key of (key, value) pairs to be located.
   *  @return  Pair of read-only (constant) iterators that possibly points to
   *           the subsequence matching given key.
   *
   *  This function improves on lower_bound() and upper_bound() by giving a more
   *  elegant and efficient solution.  It returns a pair of which the first
   *  element possibly points to the first element matching the given key
   *  and the second element possibly points to the last element matching the
   *  given key.  If unsuccessful the first element of the returned pair will
   *  contain an iterator pointing to the next greatest element or, if no such
   *  a greater element exists, to end().
   *
   *  This function only makes sense for multimaps.
  */
  pair<const_iterator,const_iterator> equal_range(const key_type& __x) const {
    return _M_t.equal_range(__x);
  }

  template <class _K1, class _T1, class _C1, class _A1>
  friend bool operator== (const map<_K1, _T1, _C1, _A1>&,
                          const map<_K1, _T1, _C1, _A1>&);
  template <class _K1, class _T1, class _C1, class _A1>
  friend bool operator< (const map<_K1, _T1, _C1, _A1>&,
                         const map<_K1, _T1, _C1, _A1>&);
};

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator==(const map<_Key,_Tp,_Compare,_Alloc>& __x,
                       const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return __x._M_t == __y._M_t;
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator<(const map<_Key,_Tp,_Compare,_Alloc>& __x,
                      const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return __x._M_t < __y._M_t;
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator!=(const map<_Key,_Tp,_Compare,_Alloc>& __x,
                       const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return !(__x == __y);
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator>(const map<_Key,_Tp,_Compare,_Alloc>& __x,
                      const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return __y < __x;
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator<=(const map<_Key,_Tp,_Compare,_Alloc>& __x,
                       const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return !(__y < __x);
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator>=(const map<_Key,_Tp,_Compare,_Alloc>& __x,
                       const map<_Key,_Tp,_Compare,_Alloc>& __y) {
  return !(__x < __y);
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline void swap(map<_Key,_Tp,_Compare,_Alloc>& __x,
                 map<_Key,_Tp,_Compare,_Alloc>& __y) {
  __x.swap(__y);
}

} // namespace std

#endif /* _CPP_BITS_STL_MAP_H */

// Local Variables:
// mode:C++
// End:
