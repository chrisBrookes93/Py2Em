#ifndef MARSHAL_UTILS_h__
#define MARSHAL_UTILS_h__

#include <dlfcn.h>
#include <Python.h>


void *LoadPython27(const char *pFilePath);
void *GetPy2Func(void *pyHandle, const char *pSymbolName);


#endif // MARSHAL_UTILS_h__