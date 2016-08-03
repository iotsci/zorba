/*
 * Copyright 2006-2016 zorba.io
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

#ifndef ZORBA_INTERNAL_TYPE_TRAITS_H
#define ZORBA_INTERNAL_TYPE_TRAITS_H

#include <zorba/config.h>

#ifdef ZORBA_TR1_IN_TR1_SUBDIRECTORY
# include <tr1/type_traits>
#else
# include <type_traits>
#endif /* ZORBA_TR1_IN_TR1_SUBDIRECTORY */

///////////////////////////////////////////////////////////////////////////////

namespace std {

#ifndef ZORBA_HAVE_ENABLE_IF
/**
 * \internal
 * Define our own version of \c enable_if since the current C++ compiler
 * doesn't have it.
 */
template<bool,typename T = void>
struct enable_if {
};

/**
 * \internal
 * Specialization of \c enable_if for the \c true case.
 */
template<typename T>
struct enable_if<true,T> {
  typedef T type;
};
#endif /* ZORBA_HAVE_ENABLE_IF */

#ifndef ZORBA_HAVE_IS_SAME
/**
 * \internal
 * Define our own version of \c is_same since the current C++ compiler doesn't
 * have it.
 */
template<typename,typename>
struct is_same {
  static const bool value = false;
};

/**
 * \internal
 * Specialization of \c is_same for the \c true case.
 */
template<typename T>
struct is_same<T,T> {
  static const bool value = true;
};
#endif /* ZORBA_HAVE_IS_SAME */

} // namespace std

///////////////////////////////////////////////////////////////////////////////

#ifndef ZORBA_HAVE_UNIQUE_PTR
namespace zorba {
namespace internal {

/**
 * \internal
 * Dummy rvalue class used by unique_ptr to prevent implicit copying.
 */
template<typename T>
class rv : public T {
  rv();
  ~rv();
  rv( rv const& );
  rv& operator=( rv const& );
  //
  // The struct and friend declaration below eliminate a "class rv<T> only
  // defines a private destructor and has no friends" warning.
  //
  struct f;
  friend struct f;
};

template<bool B>
struct bool_value {
  static bool const value = B;
};

template<typename T>
struct is_movable : bool_value<ZORBA_TR1_NS::is_convertible<T,rv<T>&>::value> {
};

template<typename T>
struct is_movable< rv<T> > : bool_value<false> {
};

} // namespace internal
} // namespace zorba
#endif /* ZORBA_HAVE_UNIQUE_PTR */

///////////////////////////////////////////////////////////////////////////////

#endif /* ZORBA_INTERNAL_TYPE_TRAITS_H */
/* vim:set et sw=2 ts=2: */
