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
#include "runtime/core/gflwor/outerfor_iterator.h"
#include "runtime/core/gflwor/common.h"

#include "runtime/core/var_iterators.h"
#include "runtime/visitors/planitervisitor.h"
#include "store/api/store.h"
#include "system/globalenv.h"
#include "store/api/item_factory.h"

using namespace zorba;

namespace zorba 
{
namespace flwor 
{

// theChild0 --> TupleIterator
// theChild1 --> InputIterator

OuterForIterator::OuterForIterator (
    short sctx,
    const QueryLoc& loc,
    const store::Item_t& aVarName,
    PlanIter_t aTupleIterator,
    PlanIter_t aInput,
    const std::vector<PlanIter_t>& aOuterForVars) 
  :
  BinaryBaseIterator<OuterForIterator, PlanIteratorState>(sctx, loc, aTupleIterator, aInput),
  theVarName (aVarName)
{
  castIterVector<ForVarIterator>(theOuterForVars, aOuterForVars);
}
  

OuterForIterator::~OuterForIterator() 
{
}


bool OuterForIterator::nextImpl (store::Item_t& aResult, PlanState& aPlanState) const 
{
  store::Item_t lItem;
  PlanIteratorState* lState;
  DEFAULT_STACK_INIT (PlanIteratorState, lState, aPlanState);
  while (consumeNext (aResult, theChild0, aPlanState)) {
    //using a if, to avoid an additional state
    if(consumeNext (lItem, theChild1, aPlanState)){
      do{
        bindVariables (lItem, theOuterForVars, aPlanState);
        STACK_PUSH (true, lState);
      } while (consumeNext (lItem, theChild1, aPlanState));
    }else{
      bindVariables (lItem, theOuterForVars, aPlanState);
      STACK_PUSH (true, lState);
    }
    theChild1->reset (aPlanState);
  }
  STACK_PUSH (false, lState);
  STACK_END (lState);
}
  
} //Namespace flwor
}//Namespace zorba
