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

#include <cstring>
#include <unicode/stsearch.h>

#include "util/unicode_util.h"
#include "zorbautils/string_util.h"
#include "zorbatypes/zstring.h"

using namespace std;
U_NAMESPACE_USE

namespace zorba {

ulong find( char const *s, size_t s_len, char const *ss, size_t ss_len,
            XQPCollator const *collator ) {
  if ( !collator ) {
    char const *const result = ::strstr( s, ss );
    return result ? result - s : zstring::npos;
  }

  unicode::string u_s, u_ss;
  unicode::to_string( s, s_len, &u_s );
  unicode::to_string( ss, ss_len, &u_ss );

  UErrorCode err = U_ZERO_ERROR;
  StringSearch search(
    u_ss, u_s, static_cast<RuleBasedCollator*>( collator->getCollator() ), NULL,
    err
  );
  if ( U_SUCCESS( err ) ) {
    for ( long pos = search.first( err );
          U_SUCCESS( err ) && pos != USEARCH_DONE;
          pos = search.next( err ) ) {
      assert(pos >= 0);
      return static_cast<ulong>(pos);
    }
  }
  return zstring::npos;
}


ulong rfind(
    char const *s,
    size_t s_len,
    char const *ss,
    size_t ss_len,
    XQPCollator const *collator ) 
{
  if ( ! collator )
  {
    zstring_b tmp;
    tmp.wrap_memory(const_cast<char*>(s), s_len);

    ulong pos = tmp.rfind(ss, ss_len);

    if (pos == zstring::npos)
      return -1;
    else
      return pos;
  }

  unicode::string u_s, u_ss;
  unicode::to_string( s, s_len, &u_s );
  unicode::to_string( ss, ss_len, &u_ss );

  UErrorCode err = U_ZERO_ERROR;

  StringSearch search(u_ss,
                      u_s,
                      static_cast<RuleBasedCollator*>(collator->getCollator()),
                      NULL,
                      err);

  if ( U_SUCCESS( err ) ) 
  {
    long pos = search.last(err);

    if(U_SUCCESS(err) && pos != USEARCH_DONE)
    {
      assert(pos >= 0);
      return static_cast<ulong>(pos);
    }
  }

  return zstring::npos;
}

bool match_part( char const *in, char const *pattern, char const *flags ) {
  unicode::regex re;
  if ( !re.compile( pattern, flags ) )
    throw zorbatypesException( pattern, ZorbatypesError::FORX0002 );
  return re.match_part( in );
}

bool match_whole( char const *in, char const *pattern, char const *flags ) {
  unicode::regex re;
  if ( !re.compile( pattern, flags ) )
    throw zorbatypesException( pattern, ZorbatypesError::FORX0002 );
  return re.match_whole( in );
}

} // namespace zorba
/* vim:set et sw=2 ts=2: */
