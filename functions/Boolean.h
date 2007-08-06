/*
 *	Copyright 2006-2007 FLWOR Foundation.
 *  Authors: Tim Kraska, David Graf
 */

#ifndef XQP_BOOLEAN_H
#define XQP_BOOLEAN_H

#include "context/common.h"
#include "functions/function.h"
#include "runtime/item_iterator.h"
#include "types/sequence_type.h"
#include "util/rchandle.h"
#include "errors/Error.h"

#include <vector>

namespace xqp {
// 9.1 Additional Boolean Constructor Functions
// 9.1.1 fn:true
// 9.1.2 fn:false
// 9.2 Operators on Boolean Values
// 9.2.1 op:boolean-equal
// 9.2.2 op:boolean-less-than
// 9.2.3 op:boolean-greater-than
// 9.3 Functions on Boolean Values
// 9.3.1 fn:not
// 

// 15.1.1 fn:boolean
class fn_boolean : public function
{
public:
	fn_boolean(const signature&);
	~fn_boolean() {}

public:
	iterator_t operator()(yy::location loc, std::vector<iterator_t>&) const;
	sequence_type_t type_check(signature&) const;
	bool validate_args(std::vector<iterator_t>&) const;
};
}

#endif
