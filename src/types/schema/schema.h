/*
 *	Copyright 2006-2008 FLWOR Foundation.
 *  Author: Cezar Andrei (cezar dot andrei at gmail dot com)
 *
 */
#ifndef ZORBA_SCHEMA_H_
#define ZORBA_SCHEMA_H_

#include "xercesIncludes.h"

#include <zorba/api_shared_types.h>

#include "common/shared_types.h"
#include "types/typeconstants.h"
#include "store/api/item.h"
#include "zorbatypes/xqpstring.h"
#include "util/hashmap.h"

namespace zorba
{

class Schema;


class Schema
{
public:
    static const char* XSD_NAMESPACE;

    /* before first use init must be called */
    static void initialize();
    /* before finishing up terminate must be called */
    static void terminate();

#ifndef ZORBA_NO_XMLSCHEMA
    Schema();
    virtual ~Schema();

    void registerXSD(const char* xsdFileName);
    void printXSDInfo(bool excludeBuiltIn = true);

    // user defined atomic type
    bool parseUserAtomicTypes(const xqpString textValue, const xqtref_t& aSourceType,
        const xqtref_t& aTargetType, store::Item_t &result);    

    /*
    * Checks if the Type with the qname exists in the schema as a user-defined type
    * if it does than return an XQType for it, if not return NULL
    */
    xqtref_t createIfExists(
        const TypeManager *manager,
        const store::Item* qname,
        TypeConstants::quantifier_t quantifier);
#endif//ZORBA_NO_XMLSCHEMA

    store::Item_t parseAtomicValue(xqtref_t type, xqpString textValue);    


private:
    static bool _isInitialized;

#ifndef ZORBA_NO_XMLSCHEMA
    XERCES_CPP_NAMESPACE::XMLGrammarPool *_grammarPool;
    hashmap<xqtref_t> *_udTypesCache;
#endif
};

} // namespace zorba

#endif /*ZORBA_SCHEMA_H_*/
/*
 * Local variables:
 * mode: c++
 * End:
 */
