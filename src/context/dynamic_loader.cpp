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
#include "context/dynamic_loader.h"

#ifdef UNIX
#  include <dlfcn.h>
#endif
#ifdef WIN32
#  include <strsafe.h>
#endif

#include "zorbaerrors/error_manager.h"


namespace zorba {

#ifdef WIN32
static void displayError(std::string& aErrorMsg)
{
  LPWSTR lpMsgBuf;
  LPWSTR lpDisplayBuf;
  DWORD dw = GetLastError();

  FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&lpMsgBuf,
                0, NULL );
  lpDisplayBuf = (LPWSTR)LocalAlloc(LMEM_ZEROINIT,
                                    (wcslen(lpMsgBuf) + 40) * sizeof(WCHAR));
  StringCchPrintfW(lpDisplayBuf,
                  LocalSize(lpDisplayBuf) / sizeof(WCHAR),
                  L"Failed with error %d: %s", dw, lpMsgBuf);

  char *buf;
  buf = new char[LocalSize(lpDisplayBuf) / sizeof(WCHAR) + 10];
  //use ACP instead of UTF8 because it is for display on Windows
  WideCharToMultiByte(CP_ACP, 0, lpDisplayBuf, -1, buf, sizeof(buf), NULL, NULL);

  aErrorMsg = buf;
  delete[] buf;

  LocalFree(lpMsgBuf);
  LocalFree(lpDisplayBuf);
}
#endif // WIN32


ExternalModule* DynamicLoader::getModule(const zstring& aFile) const
{
  // function pointer to create a module
  ExternalModule* (*createModule)() = NULL;

#ifdef WIN32
  WCHAR wpath_str[1024];
  wpath_str[0] = 0;
  if(MultiByteToWideChar(CP_UTF8,
                      0, aFile.c_str(), -1,
                      wpath_str, sizeof(wpath_str)/sizeof(WCHAR)) == 0)
  {//probably there is some invalid utf8 char, try the Windows ACP
    MultiByteToWideChar(CP_ACP,
                      0, aFile.c_str(), -1,
                      wpath_str, sizeof(wpath_str)/sizeof(WCHAR));
  }
  HMODULE handle = LoadLibraryW(wpath_str);
  if (handle == NULL)
  {
    std::string lErrorMessage;
    displayError(lErrorMessage);
    ZORBA_ERROR_DESC_OSS(API0015_CANNOT_OPEN_FILE,
                         "Cannot load the dynamic library file " << aFile
                         << ". " << lErrorMessage);
  }
  createModule = (ExternalModule* (*)())GetProcAddress(handle, "createModule");
  if (createModule == NULL)
  {
    std::string lErrorMessage;
    displayError(lErrorMessage);
    ZORBA_ERROR_DESC_OSS(API0015_CANNOT_OPEN_FILE,
                         "Function createModule not available in dynamic"
                         << " library" << aFile << ". " << lErrorMessage);
  }
#else
  void* handle = dlopen(aFile.c_str(), RTLD_NOW);
  if (!handle)
  {
    ZORBA_ERROR_DESC_OSS(API0015_CANNOT_OPEN_FILE,
                         "Cannot load the dynamic library file " << aFile
                         << ". " << dlerror());
  }

  createModule = (ExternalModule* (*)()) dlsym(handle, "createModule");
  if (createModule == NULL)
  {
    dlclose(handle);
    ZORBA_ERROR_DESC_OSS(API0015_CANNOT_OPEN_FILE,
                         "Function createModule not available in dynamic"
                         << " library" << aFile << ". " << dlerror());
  }
#endif
  if (theLibraries.find(handle) == theLibraries.end())
  {
    theLibraries.insert(handle);
  }

  return createModule();
}


DynamicLoader::~DynamicLoader()
{
  for (LibrarySet_t::const_iterator lIter = theLibraries.begin();
       lIter != theLibraries.end(); ++lIter)
  {
#ifdef WIN32
    FreeLibrary(*lIter);
#else
    dlclose(*lIter);
#endif
  }
}


} /* namespace zorba */
