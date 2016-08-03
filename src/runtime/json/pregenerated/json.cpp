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
 
// ******************************************
// *                                        *
// * THIS IS A GENERATED FILE. DO NOT EDIT! *
// * SEE .xml FILE WITH SAME NAME           *
// *                                        *
// ******************************************


#include "stdafx.h"
#include "zorbatypes/rchandle.h"
#include "zorbatypes/zstring.h"
#include "runtime/visitors/planiter_visitor.h"
#include "runtime/json/json.h"
#include "system/globalenv.h"



namespace zorba {

// <JSONtoXMLInternal>
SERIALIZABLE_CLASS_VERSIONS(JSONtoXMLInternal)

void JSONtoXMLInternal::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<JSONtoXMLInternal, PlanIteratorState>*)this);
}


void JSONtoXMLInternal::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

JSONtoXMLInternal::~JSONtoXMLInternal() {}


zstring JSONtoXMLInternal::getNameAsString() const {
  return "fn-zorba-json:json-to-xml-internal";
}
// </JSONtoXMLInternal>


// <XMLtoJSONInternal>
SERIALIZABLE_CLASS_VERSIONS(XMLtoJSONInternal)

void XMLtoJSONInternal::serialize(::zorba::serialization::Archiver& ar)
{
  serialize_baseclass(ar,
  (NaryBaseIterator<XMLtoJSONInternal, PlanIteratorState>*)this);
}


void XMLtoJSONInternal::accept(PlanIterVisitor& v) const
{
  if (!v.hasToVisit(this))
    return;

  v.beginVisit(*this);

  std::vector<PlanIter_t>::const_iterator lIter = theChildren.begin();
  std::vector<PlanIter_t>::const_iterator lEnd = theChildren.end();
  for ( ; lIter != lEnd; ++lIter ){
    (*lIter)->accept(v);
  }

  v.endVisit(*this);
}

XMLtoJSONInternal::~XMLtoJSONInternal() {}


zstring XMLtoJSONInternal::getNameAsString() const {
  return "fn-zorba-json:xml-to-json-internal";
}
// </XMLtoJSONInternal>



}


