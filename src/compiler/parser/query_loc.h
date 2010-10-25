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
#ifndef ZORBA_QUERY_LOC_H
#define ZORBA_QUERY_LOC_H

#include "zorbatypes/zstring.h"
#include "zorbaserialization/archiver.h"
#include "zorbaserialization/class_serializer.h"

namespace zorba {

/**
 * Class to save the query location of zorba elements that correspond to a code
 * snipped in the query.
 * This class is a replacement of yy::location which is generated by the bison 
 * parser generator. yy::location is used as few as possible in zorba to reduced
 * dependencies to the parser.
 */
// exported for unit testing only
  class ZORBA_DLL_PUBLIC QueryLoc : public ::zorba::serialization::SerializeBaseClass
  {

public:
  static QueryLoc null;

private:
  zstring      theFilename;
  unsigned int theLineBegin;
  unsigned int theColumnBegin;
  unsigned int theLineEnd;
  unsigned int theColumnEnd;

public:
  SERIALIZABLE_CLASS(QueryLoc)
  SERIALIZABLE_CLASS_CONSTRUCTOR(QueryLoc)
  void serialize(::zorba::serialization::Archiver& ar);

public:
  QueryLoc();

  QueryLoc(const QueryLoc& aQueryLoc);

  virtual ~QueryLoc() {}

public:
  const zstring& getFilename() const { return theFilename; }
 
  void setFilename(zstring const &aFilename) {
    theFilename = aFilename;
  }

  void setFilename(char const *aFilename) {
    theFilename = aFilename;
  }

  unsigned int getLineBegin() const { return theLineBegin; }  
   
  void setLineBegin(unsigned int aLineBegin) { theLineBegin = aLineBegin; }

  unsigned int getLineEnd() const { return theLineEnd; }    

  void setLineEnd(unsigned int aLineEnd) { theLineEnd = aLineEnd; }

  unsigned int getColumnBegin() const { return theColumnBegin; }
   
  unsigned int getColumnEnd() const { return theColumnEnd; }     

  void setColumnBegin(unsigned int aColumnBegin) { theColumnBegin = aColumnBegin; }

  void setColumnEnd(unsigned int aColumnEnd) { theColumnEnd = aColumnEnd; }

  unsigned int getLineno() const { return getLineBegin(); }
 
  bool equals(const QueryLoc& loc) const;

  bool operator==(const QueryLoc& loc) const
  {
    return equals(loc);    
  }
  
  bool operator<(const QueryLoc& loc) const
  {
    return theLineBegin < loc.getLineBegin();
  }
};


std::ostream& operator<< (std::ostream& aOstr, const QueryLoc& aQueryLoc);

} // namespace zorba

#endif
/* vim:set et sw=2 ts=2: */
