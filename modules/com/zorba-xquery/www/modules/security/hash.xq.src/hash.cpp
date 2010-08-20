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

#include <sstream>
#include <map>

#include <zorba/zorba.h>
#include <zorba/external_module.h>
#include <zorba/external_function.h>
#include <zorba/item_factory.h>
#include <zorba/singleton_item_sequence.h>
#include <zorba/base64.h>

#include "md5_impl.h"
#include "sha1.h"

namespace zorba { namespace security {

zorba::String getOneStringArgument(const StatelessExternalFunction::Arguments_t& aArgs, int aIndex)
{
  zorba::Item lItem;
  if (!(aArgs[aIndex]->next(lItem))) {
    std::stringstream lErrorMessage;
    lErrorMessage << "An empty-sequence is not allowed as "
                  << aIndex << ". parameter.";
    throw zorba::ExternalFunctionData::createZorbaException(XPTY0004,
        lErrorMessage.str().c_str(),
        __FILE__,
        __LINE__);
  }
  zorba::String lTmpString = lItem.getStringValue();
  if (aArgs[aIndex]->next(lItem)) {
    std::stringstream lErrorMessage;
    lErrorMessage << "A sequence of more then one item is not allowed as "
      << aIndex << ". parameter.";
    throw zorba::ExternalFunctionData::createZorbaException(XPTY0004,
        lErrorMessage.str().c_str(),
        __FILE__,
        __LINE__);
  }
  return lTmpString;
}

static zorba::String getNodeText(
    const StatelessExternalFunction::Arguments_t&  aArgs,
    int                 aArgumentIndex)
{
  zorba::Item lItem;
  if (!(aArgs[aArgumentIndex]->next(lItem))) {
    std::stringstream lErrorMessage;
    lErrorMessage << "An empty-sequence is not allowed as " << aArgumentIndex << ". parameter.";
    throw zorba::ExternalFunctionData::createZorbaException(XPTY0004, lErrorMessage.str().c_str(), __FILE__, __LINE__);
  }
  std::stringstream lTmpStream;
  zorba::String lText = lItem.getStringValue();
  if (aArgs[aArgumentIndex]->next(lItem)) {
    std::stringstream lErrorMessage;
    lErrorMessage << "A sequence of more then one item is not allowed as " << aArgumentIndex << ". parameter.";
    throw zorba::ExternalFunctionData::createZorbaException(XPTY0004, lErrorMessage.str().c_str(), __FILE__, __LINE__);
  }
  return lText;
}


class HashModule : public ExternalModule
{
private:
  static ItemFactory* theFactory;

protected:
  class ltstr
  {
  public:
    bool operator()(const String& s1, const String& s2) const
    {
      return s1.compare(s2) < 0;
    }
  };

  typedef std::map<String, StatelessExternalFunction*, ltstr> FuncMap_t;
  mutable FuncMap_t theFunctions;

public:
  virtual ~HashModule()
  {
    for (FuncMap_t::const_iterator lIter = theFunctions.begin();
         lIter != theFunctions.end(); ++lIter) {
      delete lIter->second;
    }
    theFunctions.clear();
  }

  virtual String
  getURI() const { return "http://www.zorba-xquery.com/modules/security/hash"; }

  virtual StatelessExternalFunction*
  getExternalFunction(String aLocalname) const;

  virtual void
  destroy()
  {
    if (!dynamic_cast<HashModule*>(this)) {
      return;
    }
    delete this;
  }

  static ItemFactory*
  getItemFactory()
  {
    if(!theFactory)
      theFactory = Zorba::getInstance(0)->getItemFactory();
    return theFactory;
  }

};

class HashFunction : public PureStatelessExternalFunction
{
protected:
  const HashModule* theModule;

  static void throwError(
      const std::string aErrorMessage,
      const XQUERY_ERROR& aErrorType)
  {
    throw zorba::ExternalFunctionData::createZorbaException(
        aErrorType,
        aErrorMessage.c_str(),
        __FILE__, __LINE__);
  }

public:
  HashFunction(const HashModule* aModule): theModule(aModule){}
  ~HashFunction(){}

  virtual String
  getLocalName() const { return "hash-unchecked"; }

  virtual zorba::ItemSequence_t
  evaluate(const Arguments_t& aArgs) const
  {
    std::string lText = (getNodeText(aArgs, 0)).c_str();
    std::string lAlg = (getOneStringArgument(aArgs, 1)).c_str();
    zorba::String lHash;
    if (lAlg == "sha1") {
      CSHA1 lSha1;
      const unsigned char* lData = (const unsigned char*) lText.c_str();
      lSha1.Update(lData, lText.size());
      lSha1.Final();
      char lRes[65];
      lSha1.GetHash((UINT_8 *)lRes);
      zorba::String lDigest(lRes);
      lHash = zorba::encoding::Base64::encode(lDigest);
    } else {
      lHash = md5(lText);
    }
    // implement here
    zorba::Item lItem;
    lItem = theModule->getItemFactory()->createString(lHash);
    return zorba::ItemSequence_t(new zorba::SingletonItemSequence(lItem));
  }

  virtual String
  getURI() const
  {
    return theModule->getURI();
  }

};

StatelessExternalFunction* HashModule::getExternalFunction(
    String aLocalname) const
{
  StatelessExternalFunction*& lFunc = theFunctions[aLocalname];
  if (!lFunc) {
    if (aLocalname.equals("hash-unchecked")) {
      lFunc = new HashFunction(this);
    }
  }
  return lFunc;
}

ItemFactory* HashModule::theFactory = 0;


} /* namespace security */ } /* namespace zorba */

#ifdef WIN32
#  define DLL_EXPORT __declspec(dllexport)
#else
#  define DLL_EXPORT __attribute__ ((visibility("default")))
#endif

extern "C" DLL_EXPORT zorba::ExternalModule* createModule() {
  return new zorba::security::HashModule();
}