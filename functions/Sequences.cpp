/**
 * @file Sequences.cpp
 * @author Paul Pedersen (pcp071098@yahoo.com)
 * @copyright 2006-2007 FLWOR Foundation (flworfound.org).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 */

#include "Sequences.h"
#include "SequencesImpl.h"

#include "context/static_context.h"
#include "dom/dom_xml_handler.h"
#include "dom/dom_nodes.h"
#include "runtime/zorba.h"
#include "store/scan_handler.h"
#include "store/xml_scanner.h"
#include "types/sequence_type.h"
#include "util/file.h"
#include "util/tracer.h"
#include "values/qname.h"
#include "values/value_factory.h"

#include <iostream>

using namespace std;
namespace xqp {


/*______________________________________________________________________
|  
| 15.1 General Functions and Operators on Sequences
|_______________________________________________________________________*/

//15.1.1 fn:boolean (effective boolean value)
//-----------------



//15.1.2 op:concatenate 
/*_______________________________________________________________________
|
|	op:concatenate($seq1 as item()*, $seq2 as item()*) as item()*
|
|	Summary: Returns a sequence consisting of the items in $seq1 followed 
|	by the items in $seq2. This function backs up the infix operator ",". 
|	If either sequence is the empty sequence, the other operand is 
|	returned. 
|________________________________________________________________________*/

op_concatenate::op_concatenate(
	const signature& sig)
:
	function(sig)
{
}

iterator_t op_concatenate::operator()(
	yy::location loc, 
	vector<iterator_t>& argv) const
{
	if (!validate_args(argv)) return NULL;
	return new concat_iterator(loc,argv);
}

bool op_concatenate::validate_args(
	vector<iterator_t>& argv) const
{
	return true;
}

sequence_type_t op_concatenate::type_check(
	signature& sig) const
{
	return xs_anyType;
}


//15.1.3 fn:index-of
/*_______________________________________________________________________
|
|	fn:index-of($seqParam as xs:anyAtomicType*,
|							$srchParam as xs:anyAtomicType) as xs:integer*
|	fn:index-of($seqParam as xs:anyAtomicType*,
|							$srchParam as xs:anyAtomicType,
|							$collation as xs:string) as xs:integer*
|	
|	Summary: Returns a sequence of positive integers giving the positions 
|	within the sequence $seqParam of items that are equal to $srchParam. 
|	
|	The collation used by the invocation of this function is determined 
|	according to the rules in 7.3.1 Collations. The collation is used when 
|	string comparison is required. 
|
|	The items in the sequence $seqParam are compared with $srchParam under 
|	the rules for the 'eq' operator. Values that cannot be compared, i.e. 
|	the 'eq' operator is not defined for their types, are considered to be 
|	distinct. If an item compares equal, then the position of that item in 
|	the sequence $seqParam is included in the result. 
|
|	If the value of $seqParam is the empty sequence, or if no item in 
|	$seqParam matches $srchParam, then the empty sequence is returned. 
|
|	The first item in a sequence is at position 1, not position 0.
|	The result sequence is in ascending numeric order.
|________________________________________________________________________*/

fn_index_of::fn_index_of(const signature& sig)
: function(sig) { }

iterator_t fn_index_of::operator()(
	yy::location loc, 
	vector<iterator_t>& argv) const
{
	if (!validate_args(argv)) return NULL;

	string collation;
	if (argv.size()==3) {
		//xs_stringValue* v_p = (xs_stringValue*)value_factory::cast_as(argv[2],xs_string);
		collation = "default_collation"; //v_p->string_value();
	}
	else {
		//static_context* sctx_p = zorba::getZorbaForCurrentThread()->get_static_context();
		//qname_value* qn_p = (qname_value*)(sctx_p->get_default_collation());
		collation = "default_collation"; //qn_p->string_value();
	}
	//return new index_of_iterator(loc, argv[0],argv[1],collation);
	return NULL;
}

bool fn_index_of::validate_args(
	vector<iterator_t>& argv) const
{
	if (argv.size()==2 || argv.size()==3) return true;
	return false;
}

sequence_type_t fn_index_of::type_check(
	signature& sig) const
{
	return xs_anyType;
}


//15.1.4 fn:empty
//---------------
fn_empty::fn_empty(const signature& sig)
: function(sig) { }

iterator_t fn_empty::operator()(
	yy::location loc, 
	vector<iterator_t>& argv) const
{
	return NULL;
}

bool fn_empty::validate_args(
	vector<iterator_t>& argv) const
{
	return true;
}

sequence_type_t fn_empty::type_check(
	signature& sig) const
{
	return xs_anyType;
}


//15.1.5 fn:exists
//----------------
fn_exists::fn_exists(const signature& sig)
: function(sig) { }

iterator_t fn_exists::operator()(
	yy::location loc, 
	vector<iterator_t>& argv) const
{
	return NULL;
}

bool fn_exists::validate_args(
	vector<iterator_t>& argv) const
{
	return true;
}

sequence_type_t fn_exists::type_check(
	signature& sig) const
{
	return xs_anyType;
}


//15.1.6 fn:distinct-values
//-------------------------
fn_distinct_values::fn_distinct_values(
	const signature& sig)
:
	function(sig) { }

iterator_t fn_distinct_values::operator()(
	yy::location loc, 
	vector<iterator_t>& argv) const
{
	return NULL;
}

bool fn_distinct_values::validate_args(
	vector<iterator_t>& argv) const
{
	return true;
}

sequence_type_t fn_distinct_values::type_check(
	signature& sig) const
{
	return xs_anyType;
}


//15.1.7 fn:insert-before
//-----------------------


//15.1.8 fn:remove
//----------------


//15.1.9 fn:reverse
//-----------------
fn_reverse::fn_reverse(const signature& sig)
: function(sig) { }

iterator_t fn_reverse::operator()(
	yy::location loc, 
	vector<iterator_t>& argv) const
{
	return NULL;
}

bool fn_reverse::validate_args(
	vector<iterator_t>& argv) const
{
	return true;
}

sequence_type_t fn_reverse::type_check(
	signature& sig) const
{
	return xs_anyType;
}


//15.1.10 fn:subsequence
//----------------------
fn_subsequence::fn_subsequence(const signature& sig)
: function(sig) { }

iterator_t fn_subsequence::operator()(
	yy::location loc, 
	vector<iterator_t>& argv) const
{
	return NULL;
}

bool fn_subsequence::validate_args(
	vector<iterator_t>& argv) const
{
	return true;
}

sequence_type_t fn_subsequence::type_check(
	signature& sig) const
{
	return xs_anyType;
}


//15.1.11 fn:unordered
//--------------------


/*______________________________________________________________________
|  
| 15.2 Functions That Test the Cardinality of Sequences
|_______________________________________________________________________*/

//15.2.1 fn:zero-or-one

//15.2.2 fn:one-or-more

//15.2.3 fn:exactly-one


/*______________________________________________________________________
|
| 15.3 Equals, Union, Intersection and Except
|_______________________________________________________________________*/

//15.3.1 fn:deep-equal

//15.3.2 op:union

//15.3.3 op:intersect

//15.3.4 op:except


/*______________________________________________________________________
|
| 15.4 Aggregate Functions
|_______________________________________________________________________*/

//15.4.1 fn:count

//15.4.2 fn:avg

//15.4.3 fn:max

//15.4.4 fn:min

//15.4.5 fn:sum


/*______________________________________________________________________
|
| 15.5 Functions and Operators that Generate Sequences
|_______________________________________________________________________*/

//15.5.1 op:to

//15.5.2 fn:id

//15.5.3 fn:idref



//15.5.4 fn:doc
//-------------
fn_doc_func::fn_doc_func(
	const signature& sig)
:
	function(sig)
{
}

iterator_t fn_doc_func::operator()(
	yy::location loc, 
	vector<iterator_t>& argv) const
{
	if (!validate_args(argv)) return NULL;
	return new doc_iterator(loc, argv[0]);
}
  
bool fn_doc_func::validate_args(
	vector<iterator_t>& argv) const
{
	if (argv.size()!=1) return false;
	return true;
}

sequence_type_t fn_doc_func::type_check(
	signature& sig) const
{
	return xs_anyType;
}


//15.5.5 fn:doc-available

//15.5.6 fn:collection



} /* namespace xqp */

