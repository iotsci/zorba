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
#ifndef ZORBA_NORMALIZE_VISITOR_H
#define ZORBA_NORMALIZE_VISITOR_H

#include <set>

#include "common/shared_types.h"

namespace zorba {

  expr_t translate (const parsenode &, CompilerCB* aCompilerCB, static_context *export_sctx = NULL, std::set<std::string> mod_stack = std::set<std::string> ());
  
} /* namespace zorba */
#endif /* ZORBA_NORMALIZE_VISITOR_H */

/*
 * Local variables:
 * mode: c++
 * End:
 */
