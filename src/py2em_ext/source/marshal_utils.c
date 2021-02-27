#include "marshal_utils.h"

/**
* Returns a bool indicating whether the Python2 interpreter is initialized. 
* This is done by checking that the Python2 binary is loaded.
*
* @return bool indicating whether the Python2 binary is loaded
**/
bool Py2IsInitialized()
{
	return pGlobPyHandle != NULL;
}

/**
* Load the libpython27.so file into memory via dlopen().
*
* @param pFilePath Name or filepath. The name must be resolvable on the LD search path
* @return bool indicating success
**/
bool LoadPython2AndInitFuncs(const char *pFilePath)
{
	// Check if we're already loaded!
	if (pGlobPyHandle)
	{
		Log("Python2 binary already loaded\n");
		return true;
	}
	Log("Loading Python2 binary: %s...", pFilePath);

	pGlobPyHandle = LoadLibrary(TEXT("python27.dll"));
		Log("END of LoadLibrary call\n");

	if (!pGlobPyHandle)
	{
		DWORD errorMessageID = GetLastError();
		Log("Failed. Error: \n");
		PyErr_SetString(PyExc_RuntimeError, "LOAD LIBRARY RETUREND NULL");
		return false;
	}
			Log("LoadPython2AndInitFuncs() success!\n");

    if (!InitializeFunctionPointers())
	{
		ClosePython27();
		return false;
	}
#ifdef _WIN32

#else

	// Clear out any current error
	(void)dlerror();
	Log("Loading Python2 binary: %s...", pFilePath);

	// This has to be DEEPBIND because within our Python3 extension there are already functions like Py_Initialize().
	// In order to make sure we call the functions in the Py2 .so file, we need to specify RTLD_DEEPBIND to place the
	// lookup scope ahead of the globals
	pGlobPyHandle = dlopen(pFilePath, RTLD_NOW | RTLD_DEEPBIND);

	char *pError = NULL;
	pError = dlerror();
	if (!pGlobPyHandle || pError) 
	{
		Log("Failed. Error: %s\n", pError);
		PyErr_SetString(PyExc_RuntimeError, pError);
		return false;
	}
	else
	{
		Log("Success.\n", pFilePath);
	}

	if (!InitializeFunctionPointers())
	{
		ClosePython27();
		return false;
	}
#endif

	return true;
}

/**
* Initialize all of the Python2 function pointers. This is done in one central place for convenience and tidiness.
*
* @return bool indicating success
**/
bool InitializeFunctionPointers()
{
	if (!pGlobPyHandle)
	{
	    Log("GLOB HANDLE IS NULL\n");
		return false;
	}
	    Log("Finding functions....\n");

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
	PY2_PyDict_Next = (PyDict_Next_t)GetPy2Func("PyDict_Next");
	PY2_PyTuple_Size = (PyTuple_Size_t)GetPy2Func("PyTuple_Size");
	PY2_Py_SetPythonHome = (Py_SetPythonHome_t)GetPy2Func("Py_SetPythonHome");
	PY2_Py_SetProgramName = (Py_SetProgramName_t)GetPy2Func("Py_SetProgramName");
	PY2_PySys_SetPath = (PySys_SetPath_t)GetPy2Func("PySys_SetPath");
	PY2_Py_GetPath = (Py_GetPath_t)GetPy2Func("Py_GetPath");
	PY2_PySys_SetArgvEx = (PySys_SetArgvEx_t)GetPy2Func("PySys_SetArgvEx");
	PY2_Py_GetProgramName = (Py_GetProgramName_t)GetPy2Func("Py_GetProgramName");
	PY2_PyInterpreterState_New = (PyInterpreterState_New_t)GetPy2Func("PyInterpreterState_New");
	PY2_PyThreadState_New = (PyThreadState_New_t)GetPy2Func("PyThreadState_New");
	PY2_PyThreadState_Swap = (PyThreadState_Swap_t)GetPy2Func("PyThreadState_Swap");
	PY2_Py_GetPythonHome = (Py_GetPythonHome_t)GetPy2Func("Py_GetPythonHome");
	PY2_Py_NoSiteFlag = (int *)GetPy2Func("Py_NoSiteFlag");

	if (!PY2_PyObject_GetIter || 
		!PY2_PyIter_Next || 
		!PY2_PyList_Size || 
		!PY2_Py_Initialize || 
		!PY2_Py_Finalize ||
		!PY2_PyLong_AsLongAndOverflow || 
		!PY2_PyLong_AsLongLongAndOverflow || 
		!PY2_PyLong_AsUnsignedLongLong || 
		!PY2_PyObject_IsTrue ||
		!PY2_PyFloat_AsDouble || 
		!PY2_PyComplex_RealAsDouble || 
		!PY2_PyComplex_ImagAsDouble || 
		!PY2_PyString_AsString || 
		!PY2_PyObject_Str ||
		!PY2_PyRun_String || 
		!PY2_PyModule_GetDict || 
		!PY2_PyImport_AddModule || 
		!PY2_PyErr_Print || 
		!PY2_PyDict_Next || 
		!PY2_PyTuple_Size ||
		!PY2_Py_SetPythonHome ||
		!PY2_Py_SetProgramName ||
		!PY2_PySys_SetPath ||
		!PY2_Py_GetPath ||
		!PY2_PySys_SetArgvEx ||
		!PY2_Py_GetProgramName ||
		!PY2_PyInterpreterState_New ||
		!PY2_PyThreadState_New ||
		!PY2_PyThreadState_Swap ||
		!PY2_Py_GetPythonHome ||
		!PY2_Py_NoSiteFlag
		) 
	{
		Log("Failed to find one of the Python2 functions.\n");
		UninitializeFunctionPointers();
		return false;
	}
    Log("InitializeFunctionPointers() returned true\n");

	return true;
}

/**
* Sets all of the Python2 function pointers to NULL
**/
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
	PY2_PyDict_Next = NULL;
	PY2_PyTuple_Size = NULL;
	PY2_Py_SetPythonHome = NULL;
	PY2_Py_SetProgramName = NULL;
	PY2_PySys_SetPath = NULL;
	PY2_Py_GetPath = NULL;
	PY2_PySys_SetArgvEx = NULL;
	PY2_Py_GetProgramName = NULL;
	PY2_PyInterpreterState_New = NULL;
	PY2_Py_GetPythonHome = NULL;
	Log("Set all of the Python2 function pointers to NULL.\n");
}

/**
* Performs a dlsym() on the loaded Python2 binary and returns the symbol asked for (or NULL)
*
* @returns void* pointer to the desired function (or NULL)
**/
void *GetPy2Func(const char *pSymbolName)
{
	void *pRetVal = NULL;
	char *pError = NULL;

	Log("Loading function %s() ...", pSymbolName);
	if (!pGlobPyHandle)
	{
		Log("GLOB HANDLE IS NULL2\n");
		return pRetVal;
	}

#ifdef _WIN32
	pRetVal = (void *)GetProcAddress(pGlobPyHandle, pSymbolName);
	if (!pRetVal)
	{
		PyErr_SetString(PyExc_RuntimeError, "GetProcAddress returned NULL");
		Log("GetProcAddress returned NULL.\n");
	}
	else
	{
		Log("Success.\n");
	}

#else

	// Clear out any current error
	(void)dlerror();

	pRetVal = dlsym(pGlobPyHandle, pSymbolName);
	pError = dlerror();

	if (pError || !pRetVal)
	{
		Log("Failed. Error: %s\n", pError);
		PyErr_SetString(PyExc_RuntimeError, pError);
	}
	else
	{
		Log("Success.\n");
	}
#endif
	return pRetVal;
}

/**
* Closes the Python2 binary.
*
* @returns bool indicating success
**/
bool ClosePython27()
{
	if (pGlobPyHandle)
	{
#ifdef _WIN32
		BOOL fFreeResult;
		fFreeResult = FreeLibrary(pGlobPyHandle);
		if (!fFreeResult)
		{
			PyErr_WarnEx(PyExc_Warning, "Call to FreeLibrary failed!", 1);
			return false;
		}
#else
		Log("Calling dlclose() on the Python2 binary...");
		int ret = dlclose(pGlobPyHandle);
		if (ret != ERROR_SUCCESS)
		{
			Log("Failed.\n");
			pGlobPyHandle = NULL;
			PyErr_WarnEx(PyExc_Warning, "dlclose() returned a non-zero return code", 1);
			return false;
		}
		else
		{
			Log("Success\n");
		}
#endif
	}
	else
	{
		Log("Python2 binary is not loaded.\n");
	}
	UninitializeFunctionPointers();
	pGlobPyHandle = NULL;
	return true;
}