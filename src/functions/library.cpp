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
#include <iostream>
#include <memory>

#include "zorbamisc/ns_consts.h"

#include "functions/library.h"
#include "functions/function.h"
#include "functions/signature.h"

#include "functions/func_accessors.h"
#include "functions/func_any_uri.h"
#include "functions/accessors_impl.h"
#include "functions/func_base64.h"
#include "functions/func_booleans.h"
#include "functions/booleans_impl.h"
#include "functions/func_collections.h"
#include "functions/func_convertors.h"
#include "functions/func_context.h"
#include "functions/func_dctx.h"
#include "functions/func_debug.h"
#include "functions/func_durations_dates_times.h"
#include "functions/durations_dates_times_impl.h"
#include "functions/func_fnerror.h"
#include "functions/func_index_ddl.h"
#include "functions/func_ic_ddl.h"
#include "functions/func_schema.h"
#include "functions/func_sctx.h"
#include "functions/func_strings.h"
#include "functions/func_sequences.h"
#include "functions/sequences_impl.h"
#include "functions/func_fnput.h"
#include "functions/func_nodes.h"
#include "functions/func_numerics.h"
#include "functions/func_qnames.h"
#include "functions/func_random.h"
#include "functions/func_maths.h"
#include "functions/func_xqdoc.h"
#include "functions/func_function_item_iter.h"

#include "functions/Numerics.h"
#include "functions/nodeid_internal.h"
#include "functions/EnclosedExpr.h"
#include "functions/VarDecl.h"
#include "functions/arithmetic.h"
//#include "functions/tuple_functions.h"
#include "functions/InternalOperators.h"
#include "functions/Alexis.h"
#include "functions/Fop.h"


namespace zorba 
{

function**  BuiltinFunctionLibrary::theFunctions = NULL;

// clear static initializer state

// dummy function to tell the windows linker to keep the library.obj
// even though it contains no public objects or functions
// this is called at initializeZorba
void library_init()
{
}

void BuiltinFunctionLibrary::create(static_context* sctx)
{
  zorba::serialization::Archiver& ar = *::zorba::serialization::ClassSerializer::getInstance()->getArchiverForHardcodedObjects();
  ar.set_loading_hardcoded_objects(true);

  theFunctions = new function*[FunctionConsts::FN_MAX_FUNC];

  populate_context_accessors(sctx);
  populate_context_any_uri(sctx);
  populate_context_accessors_impl(sctx);
  populate_context_base64(sctx);
  populate_context_booleans(sctx);
  populate_context_booleans_impl(sctx);
  populate_context_collections(sctx);
  populate_context_context(sctx);
  populate_context_convertors(sctx);
  populate_context_dctx(sctx);
  populate_context_debug(sctx);
  populate_context_durations_dates_times(sctx);
  populate_context_durations_dates_times_impl(sctx);
  populate_context_fnerror(sctx);
  populate_context_fnput(sctx);
  populate_context_index_ddl(sctx);
  populate_context_ic_ddl(sctx);
  populate_context_maths(sctx);
  populate_context_nodes(sctx);
  populate_context_numerics(sctx);
  populate_context_qnames(sctx);
  populate_context_random(sctx);
  populate_context_schema(sctx);
  populate_context_sctx(sctx);
  populate_context_strings(sctx);
  populate_context_sequences(sctx);
  populate_context_sequences_impl(sctx);
  populate_context_xqdoc(sctx);
  populate_context_function_item_iter(sctx);

  populateContext_Arithmetics(sctx);
  populateContext_Numerics(sctx);
  populateContext_DocOrder(sctx);
  populateContext_Comparison(sctx);
  populateContext_Constructors(sctx);
  populateContext_VarDecl(sctx);
  populateContext_Hoisting(sctx);
  populateContext_Alexis(sctx);
  populateContext_FOP(sctx);

  ar.set_loading_hardcoded_objects(false);
}


void BuiltinFunctionLibrary::destroy()
{
  delete [] theFunctions;
}



} /* namespace zorba */
