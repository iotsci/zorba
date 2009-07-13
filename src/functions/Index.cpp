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
#include "functions/Index.h"
#include "runtime/indexing/value_index_builder.h"
#include "runtime/indexing/value_index_probe.h"


namespace zorba
{


zop_createindex::zop_createindex(const signature& sig)
  :
  function(sig) 
{ 
}


PlanIter_t zop_createindex::codegen(short sctx, const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const
{
  return new CreateValueIndex(sctx, loc, argv[0]);
}


zop_dropindex::zop_dropindex(const signature& sig)
  :
  function(sig) 
{
}


PlanIter_t zop_dropindex::codegen(short sctx, const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const
{
  return new DropValueIndex(sctx, loc, argv[0]);
}


zop_buildindex::zop_buildindex(const signature& sig)
  :
  function(sig) 
{ 
}


PlanIter_t zop_buildindex::codegen(short sctx, const QueryLoc& loc, std::vector<PlanIter_t>& argv, AnnotationHolder &ann) const
{
  ZORBA_ASSERT(false);
}


zop_probeindexpoint::zop_probeindexpoint(const signature& sig)
  :
  function(sig) 
{
}


PlanIter_t zop_probeindexpoint::codegen(
    short sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    AnnotationHolder &ann) const
{
  return new ValueIndexPointProbe(sctx, loc, argv);
}


zop_probeindexrange::zop_probeindexrange(const signature& sig)
  : 
  function(sig) 
{
}


PlanIter_t zop_probeindexrange::codegen(
    short sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    AnnotationHolder &ann) const
{
  return new ValueIndexRangeProbe(sctx, loc, argv);
}


zop_index_session_opener::zop_index_session_opener(const signature& sig)
  :
  function(sig) 
{
}


PlanIter_t zop_index_session_opener::codegen(
    short sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    AnnotationHolder &ann) const
{
  return new ValueIndexInsertSessionOpener(sctx, loc, argv[0]);
}


zop_index_session_closer::zop_index_session_closer(const signature& sig)
  :
  function(sig) 
{
}


PlanIter_t zop_index_session_closer::codegen(
    short sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    AnnotationHolder &ann) const
{
  return new ValueIndexInsertSessionCloser(sctx, loc, argv[0]);
}


zop_index_builder::zop_index_builder(const signature& sig)
  :
  function(sig) 
{
}


PlanIter_t zop_index_builder::codegen(
    short sctx,
    const QueryLoc& loc,
    std::vector<PlanIter_t>& argv,
    AnnotationHolder &ann) const
{
  return new ValueIndexBuilder(sctx, loc, argv);
}


}
/* vim:set ts=2 sw=2: */
