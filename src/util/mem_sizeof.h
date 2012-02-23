/*
 * Copyright 2006-2008 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ZORBA_MEM_SIZEOF
#define ZORBA_MEM_SIZEOF

#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include <zorba/internal/ztd.h>

#include "zorbatypes/zstring.h"

///////////////////////////////////////////////////////////////////////////////

namespace zorba {
namespace ztd {

/**
 * \file
 * This file provides the \c mem_sizeof() function that returns the total
 * amount of memory an object is using including any dynamically allocated
 * memory it has pointers to.  Hence \c mem_sizeof() takes a reference to an
 * actual object at run-time rather than only a type at compile-time like the
 * build-in C++ \c sizeof operator does.
 *
 * For all built-in types as well as arrays, structs, classes, and unions
 * composed of only built-in types, <code>mem_sizeof(t) == sizeof(t)</code>.
 * However, for a \c std::string \s,
 * <code>mem_sizeof(s) == sizeof(s) + s.size()</code>.
 *
 * To implement this, there has to be a distinction between
 * <em>memory size</em> (<code>mem_sizeof()</code)
 * and <em>allocation size</em> (<code>alloc_sizeof()</code).
 * The latter is how much \e additional memory has been dynamically allocated
 * by an object (if any).
 *
 * For all objects of built-in types as well as arrays, structs, classes, and
 * unions composed of only built-in types, <code>alloc_sizeof(t) == 0</code>
 * because those objects don't have any additional dynamically allocated memory.
 *
 * For structs and classes that \e do have additional dynamically allocated
 * memory, there has to be a way for \c alloc_sizeof() to be able to know how
 * much memory.
 * This can be accomplished by two different methods:
 *  <ol>
 *    <li>
 *      Template partial specialization of \c size_traits.
 *    </li>
 *    <li>
 *      Deriving from \c alloc_size_provider
 *      and implementing an \c alloc_size() member function.
 *      You do this if you can (and want) to modify the class
 *      and
 *    </li>
 *  </ol>
 * Template partial specialization must be used
 * when you can not (or do not want to) modify a class.
 * However, this method may not always be able to report
 * all the dynamically allocated memory an object may be using.
 * It also has the code for a class and its template specialization
 * in different places in the code.
 *
 * Deriving from \c alloc_size_provider
 * allows you to report the precise amount of memory an object is using
 * and also has the code directly in the class.
 * However, it is intrusive and adds a virtual function.
 * (If there were no virtual functions at all,
 * then adding one might be undesirable
 * since it increases the object's size.)
 *
 * An example of template partial specialization is for \c std::string.
 * Since the source code for \c std::string can not be (easily) modified,
 * you must use template partial specialization for it.
 * \code
 *  template<>
 *  struct size_traits<std::string> {
 *    static size_t alloc_sizeof( std::string const &s ) {
 *      return s.size();
 *    }
 *  };
 * \endcode
 * This specialization, however, does not report all the memory
 * dynamically allocated by some implementations of \c std::string.
 * Some implementations use an additional "rep" object.
 * There's no way to determine whether a given implemenation uses one
 * much less how much memory it uses programatically,
 * but it's the best that can be done.
 *
 * For an example of deriving from \c alloc_size_provider,
 * see its documentation.
 */

///////////////////////////////////////////////////////////////////////////////

/**
 * Tests the given type to see if it's a class that has a member function
 * named \c alloc_size having the signature:
 * \code
 *  size_t (T::*)() const
 * \endcode
 * @tparam T The type to test.
 */
template<typename T>
class has_alloc_size : zorba::internal::ztd::sfinae_base {
  template<typename SignatureType,SignatureType> struct type_check;

  template<typename U>
  static yes& test( type_check<size_t (U::*)() const,&U::alloc_size>* );

  template<typename U>
  static no& test( ... );

public:
  static bool const value = sizeof( test<T>(0) ) == sizeof( yes );
};

/**
 * Size traits for classes that have an \c alloc_size() member function having
 * the signature:
 * \code
 *  size_t (T::*)() const
 * \endcode
 *
 * @tparam T The type to use.
 */
template<typename T,bool = has_alloc_size<T>::value>
struct size_traits {
  static size_t alloc_sizeof( T const &t ) {
    return t.alloc_size();
  }
};

/**
 * Size traits specialization for any type.
 * @tparam T The type to use.
 */
template<typename T>
struct size_traits<T,false> {
  static size_t alloc_sizeof( T const& ) {
    return 0;
  }
};

/**
 * Gets the size of all the object's data.  It does \e not
 *
 * @tparam T The type to get the data size of.
 * @param t An instance of \a T to get the data size of.
 */
template<typename T>
inline size_t alloc_sizeof( T const &t ) {
  return size_traits<T>::alloc_sizeof( t );
}

/**
 * Gets the total memory size of an object.
 *
 * @tparam T The type to get the data size of.
 * @param t An instance of \a T to get the memory size of.
 */
template<typename T>
inline size_t mem_sizeof( T const &t ) {
  return sizeof( T ) + alloc_sizeof( t );
}

////////// C++ Specializations ////////////////////////////////////////////////

/**
 * Size traits specialization for std::string.
 */
template<>
struct size_traits<std::string> {
  static size_t alloc_sizeof( std::string const &s ) {
    return s.size();
  }
};

/**
 * Size traits specialization for std::map.
 */
template<typename K,typename V>
struct size_traits<std::map<K,V>,false> {
  static size_t alloc_sizeof( std::map<K,V> const &m ) {
    size_t total_size = 0;
    for ( typename std::map<K,V>::const_iterator
          i = m.begin(); i != m.end(); ++i ) {
      total_size += mem_sizeof( i->first ) + mem_sizeof( i->second );
    }
    return total_size;
  }
};

/**
 * Size traits for a SequenceType&lt;T&gt;.
 * (This is a base class used by other specializations.)
 */
template<class SequenceType>
struct sequence_size_traits {
protected:
  static size_t sequence_sizeof( SequenceType const &s ) {
    size_t total_size = 0;
    for ( typename SequenceType::const_iterator
          i = s.begin(); i != s.end(); ++i ) {
      total_size += mem_sizeof( *i );
    }
    return total_size;
  }
};

/**
 * Size traits specialization for std::set.
 *
 * @tparam T The set's value_type.
 */
template<typename T>
struct size_traits<std::set<T>,false> : sequence_size_traits< std::set<T> > {
  static size_t alloc_sizeof( std::set<T> const &s ) {
    return sequence_sizeof( s );
  }
};

/**
 * Size traits specialization for std::stack.
 *
 * @tparam T The stack's value_type.
 */
template<typename T>
struct size_traits<std::stack<T>,false> :
  sequence_size_traits< std::stack<T> >
{
  static size_t alloc_sizeof( std::stack<T> const &s ) {
    return sequence_sizeof( s );
  }
};

/**
 * Size traits specialization for std::vector.
 *
 * @tparam T The vector's value_type.
 */
template<typename T>
struct size_traits<std::vector<T>,false> :
  sequence_size_traits< std::vector<T> >
{
  static size_t alloc_sizeof( std::vector<T> const &v ) {
    return sequence_sizeof( v );
  }
};

////////// Zorba specializations //////////////////////////////////////////////

/**
 * Size traits specialization for rstring.
 */
template<class RepType>
struct size_traits<rstring<RepType>,false> {
  static size_t alloc_sizeof( rstring<RepType> const &s ) {
    return s.size();
  }
};

////////// alloc_size_provider ////////////////////////////////////////////////

/**
 * A %alloc_size_provider provides a single function \c alloc_size().
 * An example of deriving from \c alloc_size_provider is:
 * \code
 *  class my_class : public alloc_size_provider {
 *  public:
 *    // ...
 *    size_t alloc_size() const {
 *      return alloc_size( s ) + alloc_size( t );
 *    }
 *  private:
 *    int i;
 *    std::string s;
 *    std::string t;
 *  };
 * \endcode
 * Note that doing \c alloc_size(i) isn't necessary since it returns 0.
 *
 * (Implementations of \c alloc_size()
 * should be defined in the \c .cpp file
 * since they're virtual functions.
 * They're only shown inline in the class here
 * for pedagogical reasons.)
 *
 * If you further derive classes,
 * then derived class's implementations of \c alloc_size()
 * must call their base class's implementations:
 * \code
 *  class my_derived_class : public my_class {
 *  public:
 *    // ...
 *    size_t alloc_size() const {
 *      return my_class::alloc_size() + alloc_size( u );
 *    }
 *  private:
 *    std::string u;
 *  };
 * \endcode
 */
struct alloc_size_provider {
  virtual ~alloc_size_provider() { }

  /**
   * Gets the size of all the object's dynamically allocated data (in bytes).
   * It does \e not include the size of the object itself.
   *
   * @return Returns said size.
   */
  virtual size_t alloc_size() const = 0;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ztd
} // namespace zorba
#endif /* ZORBA_MEM_SIZEOF */
/* vim:set et sw=2 ts=2: */
