#include "marshal_utils.h"


bool Py2IsInitialized()
{
	return pGlobPyHandle != NULL;
}

/**
* Load the libpython27.so file into memory via dlopen().
*
* @param pFilePath Name or filepath. The name must be resolvable on the LD search path
**/
bool LoadPython27(const char *pFilePath)
{
	// Check if we're already loaded!
	if (pGlobPyHandle)
	{
		printf("FILE ALREADY LOADED!!!!!!!!!!!!!!!!\n");
		return true;
	}

	// Clear out any current error
	(void)dlerror();

	// This has to be DEEPBIND because within our Python3 extension there are already functions like Py_Initialize().
	// In order to make sure we call the functions in the Py2 .so file, we need to specify RTLD_DEEPBIND to place the
	// lookup scope ahead of the globals
	pGlobPyHandle = dlopen(pFilePath, RTLD_NOW | RTLD_DEEPBIND);
			printf("dlopen called!!\n");


	char *pError = NULL;
	pError = dlerror();
	if (!pGlobPyHandle || pError) 
	{
		PyErr_SetString(PyExc_RuntimeError, pError);
		return false;
	}
	return true;
}

void *GetPy2Func(const char *pSymbolName)
{
	void *pRetVal = NULL;
	char *pError = NULL;

	// Clear out any current error
	(void)dlerror();

	pRetVal = dlsym(pGlobPyHandle, pSymbolName);

	pError = dlerror();
	if (pError || !pRetVal)
	{
		PyErr_SetString(PyExc_RuntimeError, pError);
	}
	return pRetVal;
}

bool ClosePython27()
{
	if (pGlobPyHandle)
	{
		int ret = dlclose(pGlobPyHandle);
		if (ret != 0)
		{
			PyErr_WarnEx(PyExc_Warning, "dlclose() returned a non-zero return code", 1);
			return false;
		}
	}
	pGlobPyHandle = NULL;
	return true;
}
