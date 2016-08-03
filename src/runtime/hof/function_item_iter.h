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
#ifndef ZORBA_RUNTIME_FUNCTION_ITEM_ITER_H
#define ZORBA_RUNTIME_FUNCTION_ITEM_ITER_H

#include "common/shared_types.h"

#include "runtime/base/narybase.h"

namespace zorba
{

class FunctionItemInfo;

typedef rchandle<FunctionItemInfo> FunctionItemInfo_t;


/*******************************************************************************
  An iterator that creates and returns a function item. The children of the
  iterator are the subplans that compute the values of the outer variables.
********************************************************************************/
class FunctionItemIterator : public NaryBaseIterator<FunctionItemIterator,
                                                     PlanIteratorState>
{
protected:
  FunctionItemInfo_t theFunctionItemInfo;

public:
  SERIALIZABLE_CLASS(FunctionItemIterator)
  SERIALIZABLE_CLASS_CONSTRUCTOR2T(FunctionItemIterator,
  NaryBaseIterator<FunctionItemIterator, PlanIteratorState>)
  void serialize(::zorba::serialization::Archiver& ar);

public:
  FunctionItemIterator(
      static_context* sctx,
      const QueryLoc& loc,
      FunctionItemInfo* fnInfo);

  virtual ~FunctionItemIterator();

  const FunctionItemInfo_t getFunctionItemInfo() const
  {
    return theFunctionItemInfo;
  }

  void accept(PlanIterVisitor& v) const;

  zstring getNameAsString() const;

  bool nextImpl(store::Item_t& result, PlanState& planState) const;
};


}//end of zorba namespace

#endif
/* vim:set et sw=2 ts=2: */
