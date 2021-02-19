#include "marshal_utils.h"

/**
* Load the libpython27.so file into memory via dlopen().
*
* @param pFilePath Name or filepath. The name must be resolvable on the LD search path
* @return Handle to the so file.
**/
void *LoadPython27(const char *pFilePath)
{
	// This has to be DEEPBIND because within our Python3 extension there are already functions like Py_Initialize().
	// In order to make sure we call the functions in the Py2 .so file, we need to specify RTLD_DEEPBIND to place the
	// lookup scope ahead of the globals
	void *pyHandle;
	pyHandle = dlopen(pFilePath, RTLD_NOW | RTLD_DEEPBIND);
	if (!pyHandle)
	{
		PyErr_SetString(PyExc_RuntimeError, dlerror());
		return NULL;
	}
	return pyHandle;
}

void *GetPy2Func(void *pyHandle, const char *pSymbolName)
{
	void *pRetVal = NULL;
	pRetVal = dlsym(pyHandle, "PyLong_AsLong");
	if (!pRetVal)
	{
		char* pErrorStr = NULL;
		sprintf(pErrorStr, "Failed to find function: %s", pSymbolName);
		PyErr_SetString(PyExc_RuntimeError, pErrorStr);
	}
	return pRetVal;
}
