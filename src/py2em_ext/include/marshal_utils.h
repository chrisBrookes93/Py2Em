#ifndef MARSHAL_UTILS_h__
#define MARSHAL_UTILS_h__

#include <dlfcn.h>
#include <Python.h>
#include <stdbool.h>

bool Py2IsInitialized();
bool LoadPython27(const char *pFilePath);
void *GetPy2Func(const char *pSymbolName);
bool ClosePython27();


/**
* Handle to the libpython2.7.so file
**/
void *pGlobPyHandle;

#endif // MARSHAL_UTILS_h__