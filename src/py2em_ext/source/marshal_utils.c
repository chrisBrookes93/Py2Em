#include "marshal_utils.h"

/*
* Handle to the Python2 binary
*/
void *pGlobPyHandle;

/*
* Python2 symbol pointers
*/
PyObject_GetIter_t             PY2_PyObject_GetIter;
PyIter_Next_t                  PY2_PyIter_Next;
PyList_Size_t                  PY2_PyList_Size;
Py_Initialize_t                PY2_Py_Initialize;
Py_Finalize_t                  PY2_Py_Finalize;
PyLong_AsLongAndOverflow_t     PY2_PyLong_AsLongAndOverflow;
PyLong_AsLongLongAndOverflow_t PY2_PyLong_AsLongLongAndOverflow;
PyLong_AsUnsignedLongLong_t    PY2_PyLong_AsUnsignedLongLong;
PyObject_IsTrue_t              PY2_PyObject_IsTrue;
PyFloat_AsDouble_t             PY2_PyFloat_AsDouble;
PyComplex_RealAsDouble_t       PY2_PyComplex_RealAsDouble;
PyComplex_ImagAsDouble_t       PY2_PyComplex_ImagAsDouble;
PyString_AsString_t            PY2_PyString_AsString;
PyObject_Str_t                 PY2_PyObject_Str;
PyRun_String_t                 PY2_PyRun_String;
PyModule_GetDict_t             PY2_PyModule_GetDict;
PyImport_AddModule_t           PY2_PyImport_AddModule;
PyErr_Print_t                  PY2_PyErr_Print;
PyDict_Next_t                  PY2_PyDict_Next;
PyTuple_Size_t                 PY2_PyTuple_Size;
Py_SetPythonHome_t             PY2_Py_SetPythonHome;
PyErr_Fetch_t                  PY2_PyErr_Fetch;
Py_NoSiteFlag_t                PY2_Py_NoSiteFlag;



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
* Load the Python2 binary into memory via dlopen/LoadLibrary and initialize the symbols.
*
* @param pFilePath Name or filepath. The name must be resolvable on the LD search path
* @return bool indicating success
**/
bool LoadPy2AndResolveSymbols(const char* pFilePath)
{
	// Check if we're already loaded!
	if (pGlobPyHandle)
	{
		Log("Python2 binary already loaded\n");
		return true;
	}

	Log("Loading Python2 binary: %s...", pFilePath);

#ifdef _WIN32

	pGlobPyHandle = LoadLibrary(TEXT(pFilePath));

	if (!pGlobPyHandle)
	{
		PY3_PyErr_Format(PyExc_RuntimeError, "Failed to find Python2 binary at %s. Error: LoadLibrary returned NULL", pFilePath);
		return false;
	}

#else

	// Clear out any current error
	(void)dlerror();

	// This has to be DEEPBIND because within our Python3 extension there are already functions like Py_Initialize().
	// In order to make sure we call the functions in the Py2 .so file, we need to specify RTLD_DEEPBIND to place the
	// lookup scope ahead of the globals
	pGlobPyHandle = dlopen(pFilePath, RTLD_NOW | RTLD_DEEPBIND);

	char* pError = NULL;
	pError = dlerror();
	if (!pGlobPyHandle || pError)
	{
		Log("Failed. Error: %s\n", pError);
		PY3_PyErr_Format(PyExc_RuntimeError, "Failed to find Python2 binary at %s. Error: %s", pFilePath, pError);
		return false;
	}
	else
	{
		Log("Success.\n", pFilePath);
	}

#endif

	if (!InitializePy2Symbols())
	{
		ClosePy27();
		return false;
	}

	return true;
}

/**
* Initialize all of the Python2 symbol pointers. This is done in one central place for convenience and tidiness.
*
* @return bool indicating success
**/
bool InitializePy2Symbols()
{
	if (!pGlobPyHandle)
	{
		Log("Python2 binary is not loaded.\n");
		return false;
	}
	Log("Finding Py2 symbols....\n");

	// Initialize all of the symbols 
	PY2_PyObject_GetIter = (PyObject_GetIter_t)GetPy2Symbol("PyObject_GetIter");
	PY2_PyIter_Next = (PyIter_Next_t)GetPy2Symbol("PyIter_Next");
	PY2_PyList_Size = (PyList_Size_t)GetPy2Symbol("PyList_Size");
	PY2_Py_Initialize = (Py_Initialize_t)GetPy2Symbol("Py_Initialize");
	PY2_Py_Finalize = (Py_Finalize_t)GetPy2Symbol("Py_Finalize");
	PY2_PyLong_AsLongAndOverflow = (PyLong_AsLongAndOverflow_t)GetPy2Symbol("PyLong_AsLongAndOverflow");
	PY2_PyLong_AsLongLongAndOverflow = (PyLong_AsLongLongAndOverflow_t)GetPy2Symbol("PyLong_AsLongLongAndOverflow");
	PY2_PyLong_AsUnsignedLongLong = (PyLong_AsUnsignedLongLong_t)GetPy2Symbol("PyLong_AsUnsignedLongLong");
	PY2_PyObject_IsTrue = (PyObject_IsTrue_t)GetPy2Symbol("PyObject_IsTrue");
	PY2_PyFloat_AsDouble = (PyFloat_AsDouble_t)GetPy2Symbol("PyFloat_AsDouble");
	PY2_PyComplex_RealAsDouble = (PyComplex_RealAsDouble_t)GetPy2Symbol("PyComplex_RealAsDouble");
	PY2_PyComplex_ImagAsDouble = (PyComplex_ImagAsDouble_t)GetPy2Symbol("PyComplex_ImagAsDouble");
	PY2_PyString_AsString = (PyString_AsString_t)GetPy2Symbol("PyString_AsString");
	PY2_PyObject_Str = (PyObject_Str_t)GetPy2Symbol("PyObject_Str");
	PY2_PyRun_String = (PyRun_String_t)GetPy2Symbol("PyRun_String");
	PY2_PyModule_GetDict = (PyModule_GetDict_t)GetPy2Symbol("PyModule_GetDict");
	PY2_PyImport_AddModule = (PyImport_AddModule_t)GetPy2Symbol("PyImport_AddModule");
	PY2_PyErr_Print = (PyErr_Print_t)GetPy2Symbol("PyErr_Print");
	PY2_PyDict_Next = (PyDict_Next_t)GetPy2Symbol("PyDict_Next");
	PY2_PyTuple_Size = (PyTuple_Size_t)GetPy2Symbol("PyTuple_Size");
	PY2_Py_SetPythonHome = (Py_SetPythonHome_t)GetPy2Symbol("Py_SetPythonHome");
	PY2_PyErr_Fetch = (PyErr_Fetch_t)GetPy2Symbol("PyErr_Fetch");
	PY2_Py_NoSiteFlag = (Py_NoSiteFlag_t)GetPy2Symbol("Py_NoSiteFlag");

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
		!PY2_PyErr_Fetch ||
		!PY2_Py_NoSiteFlag)
	{
		Log("Failed to find one of the Python2 symbols.\n");
		UninitializePy2Symbols();
		return false;
	}

	return true;
}

/**
* Sets all of the Python2 symbol pointers to NULL
**/
void UninitializePy2Symbols()
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
	PY2_PyErr_Fetch = NULL;
	PY2_Py_NoSiteFlag = NULL;
	Log("Set all of the Python2 function pointers to NULL.\n");
}

/**
* Performs a dlsym/GetProcAddress on the loaded Python2 binary and returns the symbol asked for (or NULL)
*
* @returns void* pointer to the desired function (or NULL)
**/
void* GetPy2Symbol(const char* pSymbolName)
{
	void* pRetVal = NULL;
	char* pError = NULL;

	Log("Loading function %s() ...", pSymbolName);
	if (!pGlobPyHandle)
	{
		Log("Python2 is not initialized\n");
		return pRetVal;
	}

#ifdef _WIN32
	pRetVal = (void*)GetProcAddress(pGlobPyHandle, pSymbolName);
	if (!pRetVal)
	{
		Log("Failed. GetProcAddress returned NULL.\n");
		PY3_PyErr_Format(PyExc_RuntimeError, "Failed to find Py2 symbol: %s. Error: GetProcAddress returned NULL.\n", pSymbolName);
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
		PY3_PyErr_Format(PyExc_RuntimeError, "Failed to find Py2 symbol: %s. Error: %s\n", pSymbolName, pError);
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
bool ClosePy27()
{
	if (pGlobPyHandle)
	{
		UninitializePy2Symbols();


#ifdef _WIN32
		BOOL freeResult;
		freeResult = FreeLibrary(pGlobPyHandle);
		pGlobPyHandle = NULL;
		if (!freeResult)
		{
			PY3_PyErr_WarnEx(PyExc_Warning, "Call to FreeLibrary failed!", 1);
			return false;
		}
#else
		Log("Calling dlclose() on the Python2 binary...");
		int ret = dlclose(pGlobPyHandle);
		pGlobPyHandle = NULL;
		if (ret != ERROR_SUCCESS)
		{
			Log("Failed.\n");
			PY3_PyErr_WarnEx(PyExc_Warning, "dlclose() returned a non-zero return code", 1);
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
	return true;
}