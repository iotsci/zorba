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

#include <zorba/transcode_streambuf.h>

#include "util/transcode_streambuf.h"

using namespace std;

namespace zorba {

///////////////////////////////////////////////////////////////////////////////

transcode_streambuf::exception::exception( string const &message ) :
  message_( message )
{
}

transcode_streambuf::exception::~exception() throw() {
  // out-of-line since it's virtual
}

char const* transcode_streambuf::exception::what() const throw() {
  return message_.c_str();
}

///////////////////////////////////////////////////////////////////////////////

transcode_streambuf::transcode_streambuf( char const *charset,
                                          streambuf *orig ) :
  intern_buf_( new internal::transcode_streambuf( charset, orig ) )
{
}

transcode_streambuf::~transcode_streambuf() {
  // out-of-line since it's virtual
}

void transcode_streambuf::imbue( std::locale const &loc ) {
  intern_buf_->pubimbue( loc );
}

transcode_streambuf::pos_type
transcode_streambuf::seekoff( off_type o, ios_base::seekdir d,
                             ios_base::openmode m ) {
  return intern_buf_->pubseekoff( o, d, m );
}

transcode_streambuf::pos_type
transcode_streambuf::seekpos( pos_type p, ios_base::openmode m ) {
  return intern_buf_->pubseekpos( p, m );
}

streambuf* transcode_streambuf::setbuf( char_type *p, streamsize s ) {
  intern_buf_->pubsetbuf( p, s );
  return this;
}

streamsize transcode_streambuf::showmanyc() {
  return intern_buf_->in_avail();
}

int transcode_streambuf::sync() {
  return intern_buf_->pubsync();
}

transcode_streambuf::int_type transcode_streambuf::overflow( int_type c ) {
  return intern_buf_->sputc( c );
}

transcode_streambuf::int_type transcode_streambuf::pbackfail( int_type c ) {
  return intern_buf_->sputbackc( traits_type::to_char_type( c ) );
}

transcode_streambuf::int_type transcode_streambuf::uflow() {
  return intern_buf_->sbumpc();
}

transcode_streambuf::int_type transcode_streambuf::underflow() {
  return intern_buf_->sgetc();
}

streamsize transcode_streambuf::xsgetn( char_type *to, streamsize size ) {
  return intern_buf_->sgetn( to, size );
}

streamsize transcode_streambuf::xsputn( char_type const *from,
                                       streamsize size ) {
  return intern_buf_->sputn( from, size );
}

///////////////////////////////////////////////////////////////////////////////

} // namespace zorba
/* vim:set et sw=2 ts=2: */
