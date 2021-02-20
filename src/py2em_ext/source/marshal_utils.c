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
bool LoadPython2AndInitFuncs(const char *pFilePath)
{
	// Check if we're already loaded!
	if (pGlobPyHandle)
	{
		return true;
	}

	// Clear out any current error
	(void)dlerror();

	// This has to be DEEPBIND because within our Python3 extension there are already functions like Py_Initialize().
	// In order to make sure we call the functions in the Py2 .so file, we need to specify RTLD_DEEPBIND to place the
	// lookup scope ahead of the globals
	pGlobPyHandle = dlopen(pFilePath, RTLD_NOW | RTLD_DEEPBIND);

	char *pError = NULL;
	pError = dlerror();
	if (!pGlobPyHandle || pError) 
	{
		PyErr_SetString(PyExc_RuntimeError, pError);
		return false;
	}

	if (!InitializeFunctionPointers())
	{
		ClosePython27();
		return false;
	}

	return true;
}

bool InitializeFunctionPointers()
{
	if (!pGlobPyHandle)
	{
		return false;
	}

	// Initialize all of the function pointers
 	PY2_PyObject_GetIter = (PyObject_GetIter_t)GetPy2Func("PyObject_GetIter");
 	PY2_PyIter_Next = (PyIter_Next_t)GetPy2Func("PyIter_Next");
	PY2_PyList_Size = (PyList_Size_t)GetPy2Func("PyList_Size");
	PY2_Py_Initialize = (Py_Initialize_t)GetPy2Func("Py_Initialize");
	PY2_Py_Finalize = (Py_Finalize_t)GetPy2Func("Py_Finalize");
	PY2_PyLong_AsLongAndOverflow = (PyLong_AsLongAndOverflow_t)GetPy2Func("PyLong_AsLongAndOverflow");
	PY2_PyLong_AsLongLongAndOverflow = (PyLong_AsLongLongAndOverflow_t)GetPy2Func("PyLong_AsLongLongAndOverflow");
	PY2_PyLong_AsUnsignedLongLong = (PyLong_AsUnsignedLongLong_t)GetPy2Func("PyLong_AsUnsignedLongLong");
	PY2_PyObject_IsTrue = (PyObject_IsTrue_t)GetPy2Func("PyObject_IsTrue");
	PY2_PyFloat_AsDouble = (PyFloat_AsDouble_t)GetPy2Func("PyFloat_AsDouble");
	PY2_PyComplex_RealAsDouble = (PyComplex_RealAsDouble_t)GetPy2Func("PyComplex_RealAsDouble");
	PY2_PyComplex_ImagAsDouble = (PyComplex_ImagAsDouble_t)GetPy2Func("PyComplex_ImagAsDouble");
	PY2_PyString_AsString = (PyString_AsString_t)GetPy2Func("PyString_AsString");
	PY2_PyObject_Str = (PyObject_Str_t)GetPy2Func("PyObject_Str");
	PY2_PyRun_String = (PyRun_String_t)GetPy2Func("PyRun_String");
	PY2_PyModule_GetDict = (PyModule_GetDict_t)GetPy2Func("PyModule_GetDict");
	PY2_PyImport_AddModule = (PyImport_AddModule_t)GetPy2Func("PyImport_AddModule");
	PY2_PyErr_Print = (PyErr_Print_t)GetPy2Func("PyErr_Print");

	// TODO - check that if the first fails, that the error isn't wiped out by a later one succeeding
	if (!PY2_PyObject_GetIter || !PY2_PyIter_Next || !PY2_PyList_Size || !PY2_Py_Initialize || !PY2_Py_Finalize ||
		!PY2_PyLong_AsLongAndOverflow || !PY2_PyLong_AsLongLongAndOverflow || !PY2_PyLong_AsUnsignedLongLong || !PY2_PyObject_IsTrue ||
		!PY2_PyFloat_AsDouble || !PY2_PyComplex_RealAsDouble || !PY2_PyComplex_ImagAsDouble || !PY2_PyString_AsString || !PY2_PyObject_Str ||
		!PY2_PyRun_String || !PY2_PyModule_GetDict || !PY2_PyImport_AddModule || !PY2_PyErr_Print) 
	{
		UninitializeFunctionPointers();
		return false;
	}
	return true;
}

void UninitializeFunctionPointers()
{
	PY2_PyObject_GetIter = NULL;
	PY2_PyIter_Next = NULL;
	PY2_PyList_Size = NULL;
	PY2_Py_Initialize = NULL;
	PY2_Py_Finalize = NULL;
	PY2_PyLong_AsLongAndOverflow = NULL;
	PY2_PyLong_AsLongLongAndOverflow = NULL;
	PY2_PyLong_AsUnsignedLongLong = NULL;
	PY2_PyObject_IsTrue = NULL;
	PY2_PyFloat_AsDouble = NULL;
	PY2_PyComplex_RealAsDouble = NULL;
	PY2_PyComplex_ImagAsDouble = NULL;
	PY2_PyString_AsString = NULL;
	PY2_PyObject_Str = NULL;
	PY2_PyRun_String = NULL;
	PY2_PyModule_GetDict = NULL;
	PY2_PyImport_AddModule = NULL;
	PY2_PyErr_Print = NULL;
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
	UninitializeFunctionPointers();
	pGlobPyHandle = NULL;
	return true;
}
