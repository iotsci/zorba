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
#include <fstream>

#include "zorbaerrors/Assert.h"
#include "zorbaerrors/error_messages.h"

#include "zorbatypes/zorbatypesError.h"
#include "zorbatypes/URI.h"
#include "zorbatypes/numconversions.h"

#include "api/serialization/serializer.h"

#include "system/globalenv.h"

#include "context/static_context.h"
#include "context/dynamic_context.h"
#include "context/internal_uri_resolvers.h"
#include "context/statically_known_collection.h"
#include "context/static_context_consts.h"

#include "compiler/api/compilercb.h"

#include "runtime/api/runtimecb.h"
#include "runtime/collections/CollectionsImpl.h"
#include "runtime/visitors/planiter_visitor.h"

#include "store/api/pul.h"
#include "store/api/copymode.h"
#include "store/api/item_factory.h"
#include "store/api/store.h"
#include "store/api/collection.h"

#include "types/typeops.h"


namespace zorba {

SERIALIZABLE_CLASS_VERSIONS(DcIsAvailableCollectionIterator)
END_SERIALIZABLE_CLASS_VERSIONS(DcIsAvailableCollectionIterator)


SERIALIZABLE_CLASS_VERSIONS(ZorbaIndexOfIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaIndexOfIterator)

SERIALIZABLE_CLASS_VERSIONS(DcAvailableCollectionsIterator)
END_SERIALIZABLE_CLASS_VERSIONS(DcAvailableCollectionsIterator)

SERIALIZABLE_CLASS_VERSIONS(ScIsDeclaredCollectionIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ScIsDeclaredCollectionIterator)

SERIALIZABLE_CLASS_VERSIONS(ScDeclaredCollectionsIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ScDeclaredCollectionsIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaCreateCollectionIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaCreateCollectionIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaDropCollectionIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaDropCollectionIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaDropAllCollectionsIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaDropAllCollectionsIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesFirstIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesFirstIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesLastIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesLastIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesBeforeIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesBeforeIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesAfterIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesAfterIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesAtIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaInsertNodesAtIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaRemoveNodesIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaRemoveNodesIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaRemoveNodeAtIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaRemoveNodeAtIterator)

SERIALIZABLE_CLASS_VERSIONS(FnCollectionIterator)
END_SERIALIZABLE_CLASS_VERSIONS(FnCollectionIterator)

SERIALIZABLE_CLASS_VERSIONS(ZorbaCollectionIterator)
END_SERIALIZABLE_CLASS_VERSIONS(ZorbaCollectionIterator)

NARY_ACCEPT(FnCollectionIterator);

NARY_ACCEPT(ZorbaCollectionIterator);

NARY_ACCEPT(DcIsAvailableCollectionIterator);

NARY_ACCEPT(ZorbaIndexOfIterator);

NARY_ACCEPT(DcAvailableCollectionsIterator);

NARY_ACCEPT(ScIsDeclaredCollectionIterator);

NARY_ACCEPT(ScDeclaredCollectionsIterator);

NARY_ACCEPT(ZorbaCreateCollectionIterator);

NARY_ACCEPT(ZorbaDropCollectionIterator);

NARY_ACCEPT(ZorbaDropAllCollectionsIterator);

NARY_ACCEPT(ZorbaInsertNodesFirstIterator);

NARY_ACCEPT(ZorbaInsertNodesLastIterator);

NARY_ACCEPT(ZorbaInsertNodesBeforeIterator);

NARY_ACCEPT(ZorbaInsertNodesAfterIterator);

NARY_ACCEPT(ZorbaInsertNodesAtIterator);

NARY_ACCEPT(ZorbaRemoveNodesIterator);

NARY_ACCEPT(ZorbaRemoveNodeAtIterator);

// Forward declarations
store::Collection_t getCollection(const static_context* aSctx,
                                  const store::Item_t,
                                  const QueryLoc&);

const StaticallyKnownCollection* getDeclColl(
    const static_context* aSctx,
    const store::Item_t aName,
    const QueryLoc& aLoc);

void checkNodeType(const store::Item_t& aNode, 
                   const static_context* aSctx,
                   const StaticallyKnownCollection* aColl,
                   const QueryLoc& aLoc);


/*******************************************************************************
  fn:collection() as node()*
  fn:collection($uri as xs:string?) as node()*
********************************************************************************/
FnCollectionIteratorState::FnCollectionIteratorState()
  :
  theIteratorOpened(false)
{
}

FnCollectionIteratorState::~FnCollectionIteratorState()
{
  if ( theIterator != NULL ) 
  {
    // closing the iterator is necessary here if an exception occurs
    // in the producer or if the iterator is not fully consumed
    if (theIteratorOpened) 
    {
      theIterator->close();
      theIteratorOpened = false;
    }
    theIterator = NULL;
  }
}

void FnCollectionIteratorState::init(PlanState& planState)
{
  PlanIteratorState::init(planState);
  theIterator = NULL;
}

void FnCollectionIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);

  if ( theIterator != NULL ) 
  {
    // closing the iterator is necessary here if an exception occurs
    // in the producer or if the iterator is not fully consumed
    if (theIteratorOpened) 
    {
      theIterator->close();
      theIteratorOpened = false;
    }
    theIterator = NULL;
  }
}

bool FnCollectionIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  store::Item_t       lURI, resolvedURIItem;
  store::Collection_t coll;
  xqpStringStore_t    resolvedURIString, tt;

  FnCollectionIteratorState *state;
  DEFAULT_STACK_INIT(FnCollectionIteratorState, state, planState);

  if (theChildren.size() == 1 &&
      consumeNext(lURI, theChildren[0].getp(), planState)) 
  {
    try 
    {
      tt = lURI->getStringValue();
      resolvedURIString = theSctx->resolve_relative_uri(lURI->getStringValueP(),
                                                        xqp_string()).getStore();

      GENV_ITEMFACTORY->createAnyURI(resolvedURIItem, resolvedURIString);
    }
    catch (error::ZorbaError&) 
    {
      ZORBA_ERROR_LOC_DESC(FODC0002, loc, "Error retrieving resource.");
    }
  }
  else 
  {
    resolvedURIItem = planState.theRuntimeCB->theDynamicContext->
                      get_default_collection();

    if( NULL == resolvedURIItem)
      ZORBA_ERROR_LOC_DESC(FODC0002, loc,
                           "Default collection is undefined in the dynamic context.");
  }

  coll =  theSctx->get_collection_uri_resolver()->
          resolve(resolvedURIItem, theSctx);

  if (coll == 0) 
  {
    ZORBA_ERROR_LOC_PARAM(FODC0004, loc,
                          resolvedURIItem->getStringValue()->c_str(), "");
  }

  /** return the nodes of the collection */
  state->theIterator = coll->getIterator(false);
  ZORBA_ASSERT(state->theIterator!=NULL);
  state->theIterator->open();
  state->theIteratorOpened = true;

  while(state->theIterator->next(result))
    STACK_PUSH (true, state);

  // close as early as possible
  state->theIterator->close();
  state->theIteratorOpened = false;

  STACK_END (state);
}



/*******************************************************************************
  declare function collection() as xs:boolean

  TODO descrition
********************************************************************************/
ZorbaCollectionIteratorState::ZorbaCollectionIteratorState()
  :
  theIteratorOpened(false)
{
}


ZorbaCollectionIteratorState::~ZorbaCollectionIteratorState()
{
  if ( theIterator != NULL ) 
  {
    // closing the iterator is necessary here if an exception occurs
    // in the producer or if the iterator is not fully consumed
    if (theIteratorOpened) 
    {
      theIterator->close();
      theIteratorOpened = false;
    }
    theIterator = NULL;
  }
}


void ZorbaCollectionIteratorState::init(PlanState& planState)
{
  PlanIteratorState::init(planState);
  theIterator = NULL;
}


void ZorbaCollectionIteratorState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);

  if ( theIterator != NULL ) 
  {
    // closing the iterator is necessary here if an exception occurs
    // in the producer or if the iterator is not fully consumed
    if (theIteratorOpened) {
      theIterator->close();
      theIteratorOpened = false;
    }
    theIterator = NULL;
  }
}


bool ZorbaCollectionIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  store::Item_t       lName;
  store::Collection_t coll;

  ZorbaCollectionIteratorState *state;
  DEFAULT_STACK_INIT(ZorbaCollectionIteratorState, state, planState);

  consumeNext(lName, theChildren[0].getp(), planState);

  coll = getCollection(theSctx, lName, loc);

  /** return the nodes of the collection */
  state->theIterator = coll->getIterator(false);
  ZORBA_ASSERT(state->theIterator!=NULL);
  state->theIterator->open();
  state->theIteratorOpened = true;

  while(state->theIterator->next(result))
    STACK_PUSH (true, state);

  // close as early as possible
  state->theIterator->close();
  state->theIteratorOpened = false;

  STACK_END (state);
}


/*******************************************************************************
  declare function collection-exists() as xs:boolean

  declare function collection-exists( $uri as xs:string?) as xs:boolean

  Returns true if a collection with the requested $uri is found in the collection
  pool, false otherwise.

  Error conditions:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
  - XQST0046: could not resolve uri or given uri is not a valid uri
********************************************************************************/

bool
DcIsAvailableCollectionIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  PlanIteratorState  *state;
  store::Item_t       lName;
  store::Collection_t lCollection;
  bool                res = false;

  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  consumeNext(lName, theChildren[0].getp(), planState);

  res = true;
  try {
    lCollection = getCollection(theSctx, lName, loc);
  }
  catch (error::ZorbaError& e) {
    if (e.theErrorCode != XDDY0009) {
      throw;
    }
    // collection is not available if the getCollection helper function throws error XDDY0009
    res = false;
  }

  GENV_ITEMFACTORY->createBoolean(result, res);
  STACK_PUSH(true, state );

  STACK_END (state);
}


/*******************************************************************************
  declare function index-of($target as node()) as xs:integer

  declare function index-of($uri as xs:string?,
                            $target as node()) as xs:integer

  The function will return the index of the  $target node within the collection
  identified by $uri.

  Error condition:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
  - If the specified collection does not exist, an error is raised
    (API0006_COLLECTION_NOT_FOUND - collection does not exist).
  - If the node does not belong to the given collection, an error is raised
    (API0029_NODE_DOES_NOT_BELONG_TO_COLLECTION - the node does not belong
    to the given collection).
********************************************************************************/

bool
ZorbaIndexOfIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  store::Collection_t theColl;
  store::Item_t       item;
  int                 pos = 1;

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  consumeNext(item, theChildren[0].getp(), planState);
  theColl = getCollection(theSctx, item, loc);

  if (consumeNext(item, theChildren[theChildren.size()-1].getp(), planState))
  {
    pos = theColl->indexOf(item);

    if( -1 == pos)
      ZORBA_ERROR_LOC_DESC (API0029_NODE_DOES_NOT_BELONG_TO_COLLECTION,
                            loc,
                            "The node does not belong to collection.");

    STACK_PUSH(GENV_ITEMFACTORY->createInteger(
              result,
              Integer::parseInt(pos)), state);
  }

  STACK_END (state);
}


/*******************************************************************************
  declare function list-collections() as xs:anyURI*

  The function will return a sequence of URIs of all currently known collections.
********************************************************************************/
CollectionNamesListState::~CollectionNamesListState()
{
  if ( nameItState != NULL ) {
    nameItState->close();
    nameItState = NULL;
  }
}


void CollectionNamesListState::init(PlanState& planState)
{
  PlanIteratorState::init(planState);
  nameItState = NULL;
}


void CollectionNamesListState::reset(PlanState& planState)
{
  PlanIteratorState::reset(planState);
  if ( nameItState != NULL ) {
    nameItState->close();
    nameItState = NULL;
  }
}


bool
DcAvailableCollectionsIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  CollectionNamesListState * state;
  store::Item_t              nameItem;

  DEFAULT_STACK_INIT(CollectionNamesListState, state, planState);

  for ((state->nameItState = GENV_STORE.listCollectionNames())->open ();
       state->nameItState->next(nameItem); ) 
  {
    result = nameItem;
    STACK_PUSH( true, state);
  }

  state->nameItState->close();

  STACK_END (state);
}

bool
ScIsDeclaredCollectionIterator::nextImpl(store::Item_t& aResult, PlanState& aPlanState) const
{
  PlanIteratorState *lState;
  store::Item_t      lName;

  DEFAULT_STACK_INIT(PlanIteratorState, lState, aPlanState);
  consumeNext(lName, theChildren[0].getp(), aPlanState);
  if (theSctx->lookup_collection(lName.getp()) == 0) {
    STACK_PUSH (GENV_ITEMFACTORY->createBoolean ( aResult, false ), lState);
  }
  else {
    STACK_PUSH (GENV_ITEMFACTORY->createBoolean ( aResult, true ), lState);   
  }
  STACK_END (lState);
}

bool
ScDeclaredCollectionsIterator::nextImpl(store::Item_t& aResult, PlanState& aPlanState) const
{
  CollectionNamesListState * lState;
  store::Item_t              lName;

  DEFAULT_STACK_INIT(CollectionNamesListState, lState, aPlanState);

  for ((lState->nameItState = theSctx->list_collection_names())->open ();
       lState->nameItState->next(lName); ) 
  {
    aResult = lName;
    STACK_PUSH( true, lState);
  }

  lState->nameItState->close();

  STACK_END (lState);
}


/*******************************************************************************
  declare updating function
  create-collection($name as xs:QName,$nodes as node()*) as none

  The first function will create a new, empty, collection. The second function
  will create a new collection and will add the given nodes to it.

  Error conditions:
  -  If the collection already exists, an error is raised
     (API0005_COLLECTION_ALREADY_EXISTS - collection already exists).
  - XQP0000_DYNAMIC_RUNTIME_ERROR if the argument is the empty sequence
********************************************************************************/
bool ZorbaCreateCollectionIterator::nextImpl(
    store::Item_t& result,
    PlanState& aPlanState) const
{
  store::Item_t             lName;

  store::Item_t             node;
  store::Item_t             copyNode;
  store::Collection_t       coll;
  std::auto_ptr<store::PUL> pul;

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  consumeNext(lName, theChildren[0].getp(), aPlanState);

  // check a collection is in the set of statically known collection with this name
  if ( theSctx->lookup_collection(lName.getp()) == 0 ) {
    ZORBA_ERROR_LOC_DESC_OSS(
      XDST0010, loc,
      "collection "
      << lName->getStringValue()
      << " is not declared."
    );
  }

  // check if the collection already exists
  try
  {
    coll = getCollection(theSctx, lName, loc);
  }
  catch (error::ZorbaError& e)
  {
    if (e.theErrorCode != XDDY0009) {
      throw;
    }
    // we come here if the collection is not available (but is declared)
  }

  if (coll != NULL)
  {
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0012, loc,
                             "The collection "
                             << lName->getStringValue()
                             << "is already available.");
  }

  // create the pul and add the primitive
  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  pul->addCreateCollection(lName);

  // also add some optional nodes to the collection
  if(theChildren.size() == 2) 
  {
    store::CopyMode lCopyMode;
    bool typePreserve;
    bool nsPreserve;
    bool nsInherit;
    
    typePreserve = (theSctx->construction_mode() == StaticContextConsts::cons_preserve ?
                    true : false);
    nsPreserve = (theSctx->preserve_mode() == StaticContextConsts::preserve_ns ?
                  true : false);
    nsInherit = (theSctx->inherit_mode() == StaticContextConsts::inherit_ns ?
                 true : false);

    lCopyMode.set(true, typePreserve, nsPreserve, nsInherit);

    const StaticallyKnownCollection* lDeclColl = getDeclColl(theSctx, lName, loc);;

    while (consumeNext(node, theChildren[1].getp(), aPlanState))
    {
      checkNodeType(node, theSctx, lDeclColl, loc);
      copyNode = node->copy(NULL, NULL, lCopyMode);
      pul->addInsertIntoCollection(lName,
                                   copyNode);
    }
  }

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}


/*******************************************************************************
  declare updating function delete-collection($name as xs:QName) as none

  The function will delete the collection identified by the given URI.

  Error conditions:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
  - If the collection does not exist, an error is raised.
    (API0006_COLLECTION_NOT_FOUND - collection does not exist).
********************************************************************************/
bool
ZorbaDropCollectionIterator::nextImpl(store::Item_t& result, PlanState& aPlanState) const
{
  PlanIteratorState         *state;
  store::Item_t             item;
  store::Collection_t       coll;
  std::auto_ptr<store::PUL> pul;

  DEFAULT_STACK_INIT(PlanIteratorState, state, aPlanState);

  consumeNext(item, theChildren[0].getp(), aPlanState);

  coll = getCollection(theSctx, item, loc);

  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  pul->addDropCollection(item);

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}


/*******************************************************************************
  declare updating function delete-all-collections() as none

  The function will delete all existing collections.
********************************************************************************/
bool ZorbaDropAllCollectionsIterator::nextImpl(
    store::Item_t& result,
    PlanState& planState) const
{
  store::Iterator_t             lNameIter;
  store::Item_t                 lName;
  std::auto_ptr<store::PUL>     pul;

  PlanIteratorState* state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  for ((lNameIter = GENV_STORE.listCollectionNames())->open ();
        lNameIter->next(lName);) 
  {
    pul->addDropCollection(lName);
  }

  lNameIter->close();

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}


/*******************************************************************************
  declare updating function
  insert-nodes-first($name as xs:QName, $newnode as node()*) as none

  The function will insert the given node(s) as the first node(s) of the given
  collection. If multiple nodes are inserted, the nodes remain adjacent and
  their order preserves the node ordering of the source expression.

  Error conditions:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
  - If the specified collection does not exist, an error is raised
    (API0006_COLLECTION_NOT_FOUND - collection does not exist).
********************************************************************************/
bool
ZorbaInsertNodesFirstIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  store::Collection_t              coll;
  const StaticallyKnownCollection* lDeclColl;
  store::Item_t                    lName;
  store::Item_t                    node;
  store::Item_t                    copyNode;
  std::vector<store::Item_t>       nodes;
  std::auto_ptr<store::PUL>        pul;

  store::CopyMode lCopyMode;
  bool typePreserve;
  bool nsPreserve;
  bool nsInherit;

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  typePreserve = (theSctx->construction_mode() == StaticContextConsts::cons_preserve ?
                  true : false);
  nsPreserve = (theSctx->preserve_mode() == StaticContextConsts::preserve_ns ?
                true : false);
  nsInherit = (theSctx->inherit_mode() == StaticContextConsts::inherit_ns ?
               true : false);

  lCopyMode.set(true, typePreserve, nsPreserve, nsInherit);

  consumeNext(lName, theChildren[0].getp(), planState);

  lDeclColl = getDeclColl(theSctx, lName, loc);
  coll = getCollection(theSctx, lName, loc);

  // checking collection modifiers
  switch(lDeclColl->getCollectionModifier()) {
  case StaticContextConsts::const_:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0003, loc, 
      "insert-nodes-first has a const collection as first argument.");
    break;
  case StaticContextConsts::append_only:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0004, loc, 
      "insert-nodes-first has a append-only collection as first argument.");
    break;
  case StaticContextConsts::queue:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0005, loc, 
      "insert-nodes-first has a queue collection as first argument.");
    break;
  case StaticContextConsts::mutable_coll:
    // good to go
    break;
  }

  while (consumeNext(node, theChildren[theChildren.size()-1].getp(), planState))
  {
    checkNodeType(node, theSctx, lDeclColl, loc);
    copyNode = node->copy(NULL, NULL, lCopyMode);
    nodes.push_back(copyNode);
  }

  // create the pul and add the primitive
  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  pul->addInsertFirstIntoCollection(lName, nodes);

  // this should not be necessary. we reset everything in the sequential iterator
  theChildren[theChildren.size()-1]->reset(planState);

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}


/*******************************************************************************
  declare updating function
  insert-nodes-last($name as xs:QName, $newnode as node()*) as none

  The function will insert the given node(s) as the last node(s) of the given
  collection. If multiple nodes are inserted, the nodes remain adjacent and 
  their order preserves the node ordering of the source expression.

  Error conditions:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
  - If the specified collection does not exist, an error is raised
    (API0006_COLLECTION_NOT_FOUND - collection does not exist).
  - If the node is already in the collection, an error is raised
    (API0031_NODE_ALREADY_IN_COLLECTION) 
********************************************************************************/
bool
ZorbaInsertNodesLastIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  store::Collection_t              coll;
  const StaticallyKnownCollection* lDeclColl;
  store::Item_t                    lName;
  store::Item_t                    node;
  store::Item_t                    copyNode;
  std::vector<store::Item_t>       nodes;
  std::auto_ptr<store::PUL>        pul;

  store::CopyMode lCopyMode;
  bool typePreserve;
  bool nsPreserve;
  bool nsInherit;

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  typePreserve = (theSctx->construction_mode() == StaticContextConsts::cons_preserve ?
                  true : false);
  nsPreserve = (theSctx->preserve_mode() == StaticContextConsts::preserve_ns ?
                true : false);
  nsInherit = (theSctx->inherit_mode() == StaticContextConsts::inherit_ns ?
               true : false);

  lCopyMode.set(true, typePreserve, nsPreserve, nsInherit);

  consumeNext(lName, theChildren[0].getp(), planState);

  lDeclColl = getDeclColl(theSctx, lName, loc);
  coll = getCollection(theSctx, lName, loc);

  // checking collection modifiers
  switch(lDeclColl->getCollectionModifier()) {
  case StaticContextConsts::const_:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0003, loc, 
      "insert-nodes-last has a const collection as first argument.");
    break;
  case StaticContextConsts::append_only:
  case StaticContextConsts::queue:
  case StaticContextConsts::mutable_coll:
    // good to go
    break;
  }

  while (consumeNext(node, theChildren[theChildren.size()-1].getp(), planState))
  {
    checkNodeType(node, theSctx, lDeclColl, loc);
    copyNode = node->copy(NULL, NULL, lCopyMode);
    nodes.push_back(copyNode);
  }

  // create the pul and add the primitive
  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  pul->addInsertLastIntoCollection(lName, nodes);

  // this should not be necessary. we reset everything in the sequential iterator
  theChildren[theChildren.size()-1]->reset(planState);

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}


/*******************************************************************************
  declare updating function
  insert-nodes-before($name as xs:QName,
                      $target   as node()+,
                      $newnode  as node()*) as none


  The inserted nodes become the preceding (or following) nodes of the target.
  The $target should be a non-updating expression (e.g. an XPath expression)
  identifying a node that is part of the given collection at the top-level.
  If the expression returns more than one node, the first one is used. If 
  multiple nodes are inserted by a single insert expression, the nodes remain
  adjacent and their order preserves the node ordering of the source expression.

  Error conditions:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
  - If the specified collection does not exist, an error is raised
    (API0006_COLLECTION_NOT_FOUND - collection does not exist).
  - If the target node is not part of the collection, an error is raised
    (API0029_NODE_DOES_NOT_BELONG_TO_COLLECTION - the node does not belong
    to the given collection).
  - If any of the new nodes is already part of the collection an error is
    raised (API0031_NODE_ALREADY_IN_COLLECTION).
********************************************************************************/
bool
ZorbaInsertNodesBeforeIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  store::Collection_t              coll;
  const StaticallyKnownCollection* lDeclColl;
  store::Item_t                    lName;
  store::Item_t                    targetNode, tmpItem;
  store::Item_t                    node;
  store::Item_t                    copyNode;
  std::vector<store::Item_t>       nodes;
  std::auto_ptr<store::PUL>        pul;

  store::CopyMode lCopyMode;
  bool typePreserve;
  bool nsPreserve;
  bool nsInherit;

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  typePreserve = (theSctx->construction_mode() == StaticContextConsts::cons_preserve ?
                  true : false);
  nsPreserve = (theSctx->preserve_mode() == StaticContextConsts::preserve_ns ?
                true : false);
  nsInherit = (theSctx->inherit_mode() == StaticContextConsts::inherit_ns ?
               true : false);

  lCopyMode.set(true, typePreserve, nsPreserve, nsInherit);

  consumeNext(lName, theChildren[0].getp(), planState);

  lDeclColl = getDeclColl(theSctx, lName, loc);
  coll = getCollection(theSctx, lName, loc);

  // checking collection modifiers
  switch(lDeclColl->getCollectionModifier()) {
  case StaticContextConsts::const_:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0003, loc, 
      "insert-nodes-before has a const collection as first argument.");
    break;
  case StaticContextConsts::append_only:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0004, loc, 
      "insert-nodes-before has a append-only collection as first argument.");
    break;
  case StaticContextConsts::queue:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0005, loc, 
      "insert-nodes-before has a queue collection as first argument.");
    break;
  case StaticContextConsts::mutable_coll:
    // good to go
    break;
  }

  if(!consumeNext(targetNode, theChildren[theChildren.size()-2].getp(), planState)) 
  {
    ZORBA_ERROR_LOC_DESC(XQP0000_DYNAMIC_RUNTIME_ERROR, loc,
                         "The empty-sequence is not allowed as second argument to insert-nodes-before");
  }

  if(consumeNext(tmpItem, theChildren[theChildren.size()-2].getp(), planState)) 
  {
    ZORBA_ERROR_LOC_DESC(XQP0000_DYNAMIC_RUNTIME_ERROR, loc,
                         "A sequence with more then one item is not allowed as second argument to insert-nodes-before");
  }


  if (coll->indexOf(targetNode.getp()) == -1) 
  {
    ZORBA_ERROR_LOC_DESC_OSS(XQP0000_DYNAMIC_RUNTIME_ERROR, loc,
                         "The target node passed as second parameter to insert-nodes-before does not exist in the given collection "
                         << lName->getStringValue());
  }

  while (consumeNext(node, theChildren[theChildren.size()-1].getp(), planState))
  {
    checkNodeType(node, theSctx, lDeclColl, loc);
    copyNode = node->copy(NULL, NULL, lCopyMode);
    nodes.push_back(copyNode);
  }

  // create the pul and add the primitive
  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  pul->addInsertBeforeIntoCollection(lName,
                                     targetNode,
                                     nodes);

  // this should not be necessary. we reset everything in the sequential iterator
  theChildren[theChildren.size()-2]->reset(planState);
  theChildren[theChildren.size()-1]->reset(planState);

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}


/*******************************************************************************
  declare updating function insert-nodes-after( $name      as xs:QName,
                                                $target   as node()+,
                                                $newnode  as node()*) as none

  The inserted nodes become the following nodes of the $target.
  The $target should be a non-updating expression (e.g. an XPath expression)
  identifying a node that is part of the given collection at the top-level.
  If the expression returns more than one node, the first one is used.
  A predicate can be used to select a different node in this case.
  If multiple nodes are inserted by a single insert expression,
  the nodes remain adjacent and their order preserves the node ordering of
  the source expression.

Error conditions:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
********************************************************************************/
bool
ZorbaInsertNodesAfterIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  store::Collection_t              coll;
  const StaticallyKnownCollection* lDeclColl;
  store::Item_t                    lName;
  store::Item_t                    targetNode;
  store::Item_t                    tmpItem;
  store::Item_t                    node;
  store::Item_t                    copyNode;
  std::vector<store::Item_t>       nodes;
  std::auto_ptr<store::PUL>        pul;

  store::CopyMode lCopyMode;
  bool typePreserve;
  bool nsPreserve;
  bool nsInherit;

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  typePreserve = (theSctx->construction_mode() == StaticContextConsts::cons_preserve ?
                  true : false);
  nsPreserve = (theSctx->preserve_mode() == StaticContextConsts::preserve_ns ?
                true : false);
  nsInherit = (theSctx->inherit_mode() == StaticContextConsts::inherit_ns ?
               true : false);

  lCopyMode.set(true, typePreserve, nsPreserve, nsInherit);

  consumeNext(lName, theChildren[0].getp(), planState);

  lDeclColl = getDeclColl(theSctx, lName, loc);
  coll = getCollection(theSctx, lName, loc);

  // checking collection modifiers
  switch(lDeclColl->getCollectionModifier()) {
  case StaticContextConsts::const_:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0003, loc, 
      "insert-nodes-after has a const collection as first argument.");
    break;
  case StaticContextConsts::append_only:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0004, loc, 
      "insert-nodes-after has a append-only collection as first argument.");
    break;
  case StaticContextConsts::queue:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0005, loc, 
      "insert-nodes-after has a queue collection as first argument.");
    break;
  case StaticContextConsts::mutable_coll:
    // good to go
    break;
  }

  if(!consumeNext(targetNode, theChildren[theChildren.size()-2].getp(), planState)) 
  {
    ZORBA_ERROR_LOC_DESC(XQP0000_DYNAMIC_RUNTIME_ERROR, loc, 
                         "The empty-sequence is not allowed as second argument to insert-nodes-after");
  }

  if(consumeNext(tmpItem, theChildren[theChildren.size()-2].getp(), planState)) 
  {
    ZORBA_ERROR_LOC_DESC(XQP0000_DYNAMIC_RUNTIME_ERROR, loc, 
                         "A sequence with more then one item is not allowed as second argument to insert-nodes-after");
  }

  if (coll->indexOf(targetNode.getp()) == -1) 
  {
    ZORBA_ERROR_LOC_DESC_OSS(XQP0000_DYNAMIC_RUNTIME_ERROR, loc, 
                         "The target node passed as second parameter to insert-nodes-before does not exist in the given collection "
                         << lName->getStringValue());
  }

  // create the pul and add the primitive
  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  while (consumeNext(node, theChildren[theChildren.size()-1].getp(), planState))
  {
    checkNodeType(node, theSctx, lDeclColl, loc);
    copyNode = node->copy(NULL, NULL, lCopyMode);
    nodes.push_back(copyNode);
  }

  pul->addInsertAfterIntoCollection(lName,
                                    targetNode,
                                    nodes);

  // this should not be necessary. we reset everything in the sequential iterator
  theChildren[theChildren.size()-2]->reset(planState);
  theChildren[theChildren.size()-1]->reset(planState);

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}


/*******************************************************************************
  declare updating function
  insert-nodes-at($name as xs:QName,
                  $position as xs:integer,
                  $newnode  as node()*) as none

  Inserts the node(s) into the given collection, at the specified position.
  If $position is negative, the node(s) will be inserted at the beginning of
  the collection. If $position is greater or equal to the number of nodes in
  the collection, the node(s) will be inserted as the last node(s) of the
  collection.

  Error conditions:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
  - If the specified collection does not exist, an error is raised
    (API0006_COLLECTION_NOT_FOUND - collection does not exist).
  - If the node is already part of the collection an error is raised
    (API0031_NODE_ALREADY_IN_COLLECTION).
********************************************************************************/
bool
ZorbaInsertNodesAtIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  store::Collection_t              coll;
  const StaticallyKnownCollection* lDeclColl;
  store::Item_t                    lName, itemPos, tmpItem;
  store::Item_t                    node;
  store::Item_t                    copyNode;
  std::vector<store::Item_t>       nodes;
  std::auto_ptr<store::PUL>        pul;
  xqp_uint                         pos;

  store::CopyMode lCopyMode;
  bool typePreserve;
  bool nsPreserve;
  bool nsInherit;

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  typePreserve = (theSctx->construction_mode() == StaticContextConsts::cons_preserve ?
                  true : false);
  nsPreserve = (theSctx->preserve_mode() == StaticContextConsts::preserve_ns ?
                true : false);
  nsInherit = (theSctx->inherit_mode() == StaticContextConsts::inherit_ns ?
               true : false);

  lCopyMode.set(true, typePreserve, nsPreserve, nsInherit);

  consumeNext(lName, theChildren[0].getp(), planState);

  lDeclColl = getDeclColl(theSctx, lName, loc);
  coll = getCollection(theSctx, lName, loc);

  // checking collection modifiers
  switch(lDeclColl->getCollectionModifier()) {
  case StaticContextConsts::const_:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0003, loc, 
      "insert-nodes-at has a const collection as first argument.");
    break;
  case StaticContextConsts::append_only:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0004, loc, 
      "insert-nodes-at has a append-only collection as first argument.");
    break;
  case StaticContextConsts::queue:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0005, loc, 
      "insert-nodes-at has a queue collection as first argument.");
    break;
  case StaticContextConsts::mutable_coll:
    // good to go
    break;
  }

  if(!consumeNext(itemPos, theChildren[theChildren.size()-2].getp(), planState)) 
  {
    ZORBA_ERROR_LOC_DESC(XQP0000_DYNAMIC_RUNTIME_ERROR, loc, 
                         "The empty-sequence is not allowed as second argument to insert-nodes-at");
  }

  if(consumeNext(tmpItem, theChildren[theChildren.size()-2].getp(), planState)) 
  {
    ZORBA_ERROR_LOC_DESC(XQP0000_DYNAMIC_RUNTIME_ERROR, loc, 
                         "A sequence with more then one item is not allowed as second argument to insert-nodes-at");
  }

  if(itemPos->getIntegerValue() < Integer::zero())
  {
    ZORBA_ERROR_LOC_DESC_OSS(XQP0000_DYNAMIC_RUNTIME_ERROR, loc, 
                             "The target position passed as second argument to insert-nodes-at (" 
                              << itemPos->getStringValue() << ") must be positive");
  }

  NumConversions::strToUInt(itemPos->getIntegerValue().toString(), pos);

  // create the pul and add the primitive
  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  while (consumeNext(node, theChildren[theChildren.size()-1].getp(), planState))
  {
    checkNodeType(node, theSctx, lDeclColl, loc);
    copyNode = node->copy(NULL, NULL, lCopyMode);
    nodes.push_back(copyNode);
  }

  pul->addInsertAtIntoCollection(lName,
                                 pos,
                                 nodes);

  // this should not be necessary. we reset everything in the sequential iterator
  theChildren[theChildren.size()-2]->reset(planState);
  theChildren[theChildren.size()-1]->reset(planState);

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}


/*******************************************************************************
  declare updating function remove-nodes($name     as xs:QName,
                                         $target  as node()+) as none

  The function will remove the node(s) identified by the $target expression
  from the given collection. The nodes themselves will not be deleted and
  they may remain as parts of other collections. If the expression identifies
  more than one node, all of them will be removed from the collection. 

  Error conditions:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
  - If the specified collection does not exist, an error is raised
    (API0006_COLLECTION_NOT_FOUND - collection does not exist).
  - If the given expression points to a node that is not part of the collection,
    an error is raised (API0029_NODE_DOES_NOT_BELONG_TO_COLLECTION - the node
    does not belong to the given collection). 
********************************************************************************/
bool
ZorbaRemoveNodesIterator::nextImpl(store::Item_t& result, PlanState& planState) const
{
  store::Collection_t              coll;
  const StaticallyKnownCollection* lDeclColl;
  store::Item_t                    lName;
  store::Item_t                    node;
  std::vector<store::Item_t>       nodes;
  std::auto_ptr<store::PUL>        pul;

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  consumeNext(lName, theChildren[0].getp(), planState);

  lDeclColl = getDeclColl(theSctx, lName, loc);
  coll = getCollection(theSctx, lName, loc);

  while (consumeNext(node, theChildren[theChildren.size()-1].getp(), planState)) 
  {
    if (coll->indexOf(node.getp()) == -1)
      ZORBA_ERROR_LOC_DESC_OSS(API0029_NODE_DOES_NOT_BELONG_TO_COLLECTION, loc, 
                           "The node passed as second parameter to remove-nodes does not exist in the given collection "
                           << lName->getStringValue());

    nodes.push_back(node);
  }

  // checking collection modifiers
  switch(lDeclColl->getCollectionModifier()) {
  case StaticContextConsts::const_:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0003, loc, 
      "remove-nodes has a const collection as first argument.");
    break;
  case StaticContextConsts::append_only:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0004, loc, 
      "remove-nodes has a append-only collection as first argument.");
    break;
  case StaticContextConsts::queue:
    // checks if the deleted items are a prefix of the collection
    for (size_t i = 0; i < nodes.size(); ++i) {
      store::Item* lCurToDelete = nodes[i].getp();
      store::Item* lNodeAtPos = coll->nodeAt(i+1).getp();
      if (lCurToDelete != lNodeAtPos)
        ZORBA_ERROR_LOC_DESC_OSS(XDDY0005, loc, 
          "remove-nodes has a queue collection as first argument "
          << "and a sequence as second argument that is not a prefix of the collection.");
    }
    break;
  case StaticContextConsts::mutable_coll:
    // good to go
    break;
  }

  // create the pul and add the primitive
  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  pul->addRemoveFromCollection(lName, nodes);

  // this should not be necessary. we reset everything in the sequential iterator
  theChildren[theChildren.size()-1]->reset(planState);

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}


/*******************************************************************************
  declare updating function
  remove-node-at($name as xs:QName, $position as xs:integer) as none

  The function will remove from the given collection the node positioned at
  $position.

  Error conditions:
  - If the collection URI is empty and the default collection
    is not defined in the dynamic context, FODC0002 is raised
  - If the specified collection does not exist, an error is raised 
    (API0006_COLLECTION_NOT_FOUND - collection does not exist).
  - If the collection has fewer nodes than $position + 1, nothing is removed
    and an error is raised (API0030_NO_NODE_AT_GIVEN_POSITION - there is no
    node at the given position, the collection has fewer nodes). 
********************************************************************************/
bool ZorbaRemoveNodeAtIterator::nextImpl(
    store::Item_t& result,
    PlanState& planState) const
{
  store::Collection_t              coll;
  const StaticallyKnownCollection* lDeclColl;
  store::Item_t                    lName;
  store::Item_t                    posItem;
  store::Item_t                    tmpItem;
  uint32_t                         lpos;
  std::auto_ptr<store::PUL>        pul;

  PlanIteratorState *state;
  DEFAULT_STACK_INIT(PlanIteratorState, state, planState);

  consumeNext(lName, theChildren[0].getp(), planState);

  lDeclColl = getDeclColl(theSctx, lName, loc);
  coll = getCollection(theSctx, lName, loc);

  consumeNext(posItem, theChildren[theChildren.size()-1].getp(), planState);

  if(posItem->getIntegerValue() <= Integer::zero())
  {
    ZORBA_ERROR_LOC_DESC(XDDY0014, loc, 
                         "The position parameter of remove-node-at must be positive.");
  }

  NumConversions::strToUInt(posItem->getIntegerValue().toString(),lpos);

  if (coll->size() < lpos)
  {
    ZORBA_ERROR_LOC_DESC(XDDY0014, loc, 
                         "The size of the collection is smaller then the parameter passed as second argument to remove-node-at");
  }


  // checking collection modifiers
  switch(lDeclColl->getCollectionModifier()) {
  case StaticContextConsts::const_:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0003, loc, 
      "remove-node-at has a const collection as first argument.");
    break;
  case StaticContextConsts::append_only:
    ZORBA_ERROR_LOC_DESC_OSS(XDDY0004, loc, 
      "remove-node-at has a append-only collection as first argument.");
    break;
  case StaticContextConsts::queue:
    if (lpos != 1) {
      ZORBA_ERROR_LOC_DESC_OSS(XDDY0005, loc, 
        "remove-node-at has a queue collection as first argument and an "
        << "integer different than 1 as second argument.");
    }
    break;
  case StaticContextConsts::mutable_coll:
    // good to go
    break;
  }



  // create the pul and add the primitive
  pul.reset(GENV_ITEMFACTORY->createPendingUpdateList());

  pul->addRemoveAtFromCollection(lName, lpos);

  // this should not be necessary. we reset everything in the sequential iterator
  theChildren[theChildren.size()-1]->reset(planState);

  result = pul.release();
  STACK_PUSH( result != NULL, state);

  STACK_END (state);
}




store::Collection_t getCollection(
    const static_context* aSctx,
    const store::Item_t aName,
    const QueryLoc& aLoc)
{
  if (aSctx->lookup_collection(aName.getp()) == 0) {
    ZORBA_ERROR_LOC_PARAM_OSS(XDST0010, aLoc, aName->getStringValue(),
                              "The collection with name " 
                              << aName->getStringValue() 
                              << " is not declared.");
  }

  store::Collection_t lCollection = GENV_STORE.getCollection(aName);
  if (lCollection == NULL) 
  {
    ZORBA_ERROR_LOC_PARAM_OSS(XDDY0009, aLoc, aName->getStringValue(),
                              "The collection with name " 
                              << aName->getStringValue() 
                              << " is not available.");
  }

  return lCollection;
}

const StaticallyKnownCollection* getDeclColl(
    const static_context* aSctx,
    const store::Item_t aName,
    const QueryLoc& aLoc)
{
  const StaticallyKnownCollection* lDeclColl = aSctx->lookup_collection(aName);
  if (lDeclColl == 0) {
    ZORBA_ERROR_LOC_DESC_OSS(XDST0010, aLoc,
                             "The requested collection with name "
                             << aName->getStringValue()
                             << " is not declared.");
  }
  return lDeclColl;
}

void checkNodeType(const store::Item_t& aNode, 
                   const static_context* aSctx,
                   const StaticallyKnownCollection* aColl,
                   const QueryLoc& aLoc)
{
  const TypeManager* lTypeManager = aSctx->get_typemanager();
  if (!TypeOps::is_treatable(aNode, *(aColl->getNodeType()), lTypeManager)) {
    ZORBA_ERROR_LOC_DESC_OSS(XPTY0004, aLoc, 
      "Cannot treat "
      << TypeOps::toString(*lTypeManager->create_value_type(aNode))
      << " as " << TypeOps::toString(*(aColl->getNodeType())));
  }
}



} /* namespace zorba */
