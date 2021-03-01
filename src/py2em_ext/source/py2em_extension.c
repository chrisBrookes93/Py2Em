#include "py2em_extension.h"

/**
* Returns a bool indicating whether the Python2 emulator is initialized
*
* @param pSelf Unused
* @param pArgs Unused
* @return Python-encoded boolean indicating initialized state
**/
static PyObject* IsInitialized(PyObject* pSelf, PyObject* pArgs)
{
	Log("Enter C IsInitialized\n");
	return PY3_Py_BuildValue("O", Py2IsInitialized() ? Py_True : Py_False);
}

/**
* Invoke PyRun_String() from the loaded python binary
* This code is based upon Python/pythonrun.c#PyRun_SimpleStringFlags(). Note that PyRun_SimpleStringFlags()
* does not Py_DECREF the pointers returned by PyImport_AddModule("__main__") and PyModule_GetDict(), so we don't either.
*
* @param pCommand Python command to execute
* @param start Evaluation mode (e.g. Py_eval_input)
* @return Python-encoded result
**/
PyObject* RunString(const char* pCommand, int start)
{
	Log("Enter C RunString\n");
	PyObject* pRetVal;
	PyObject* pMainMod;
	PyObject* pLocalsGlobals;
	PyObject* pRunStrRes;
	char* pErrorStr;

	Log("Importing __main__ to get the locals/globals...");

	// Examples in Python/pythonrun.c#PyRun_SimpleStringFlags() does not DECREF this object (or the dict reference below)
	pMainMod = PY2_PyImport_AddModule("__main__");
	if (!pMainMod)
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to add module __main__");
		return NULL;
	}
	Log("Success.\nObtaining the variable dictionary...");

	pLocalsGlobals = PY2_PyModule_GetDict(pMainMod);
	if (!pLocalsGlobals)
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to get a handle to __main__ to obtain the local/global variables");
		return NULL;
	}
	Log("Success.\nCalling PyRun_String()...");

	pRunStrRes = PY2_PyRun_String(pCommand, start, pLocalsGlobals, pLocalsGlobals);
	if (!pRunStrRes)
	{
		PyObject* pType, * pValue, * pTraceback;
		Log("Failed. Resolving exception...\n");
		PY2_PyErr_Fetch(&pType, &pValue, &pTraceback);
		if (pValue)
		{
			PyObject* pObjErrStr;
			pObjErrStr = PY2_PyObject_Str(pValue);
			pErrorStr = PY2_PyString_AsString(pObjErrStr);
			PY2_Py_XDECREF(pType);
			PY2_Py_XDECREF(pValue);
			PY2_Py_XDECREF(pTraceback);
			PY2_Py_XDECREF(pObjErrStr);
		}
		else
		{
			pErrorStr = "Py2 Exception occurred but failed to resolve it";
		}
		PY3_PyErr_SetString(PyExc_RuntimeError, pErrorStr);
		return NULL;
	}
	Log("Success.\n");

	if (start == Py_eval_input)
	{
		Log("Marshalling return data...\n");
		pRetVal = MarshalObjectPy2ToPy3(pRunStrRes);
		PY2_Py_XDECREF(pRunStrRes);
		return pRetVal;
	}
	else
	{
		PY2_Py_XDECREF(pRunStrRes);
		// We are not evaluating, so return an empty (but not NULL) value
		return PY3_Py_BuildValue("");
	}
}

/**
* Initialize the Python2 Interpreter. This is done by dynamically loading the shared library and invoking Py_Initialize()
*
* @param pSelf Unused
* @param pArgs Argument dictionary
* @return Python-encoded empty value
**/
static PyObject* Initialize(PyObject* pSelf, PyObject* pArgs)
{
	Log("Enter C Initialize()\n");

	char* pPy2BinaryPath;
	char* pPy2Home;

	if (Py2IsInitialized())
	{
		PY3_PyErr_WarnEx(PyExc_Warning, "Interpreter is already Initialized.", 1);
		return PY3_Py_BuildValue("");
	}

	if (!PY3_PyArg_ParseTuple(pArgs, "ss", &pPy2BinaryPath, &pPy2Home))
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}

	if (!LoadPy2AndResolveSymbols(pPy2BinaryPath))
	{
		// LoadPy2AndResolveSymbols() should have set the error
		return NULL;
	}
	if (pPy2Home && strcmp(pPy2Home, "") != 0)
	{
		Log("Setting Python home to: %s\n", pPy2Home);
		PY2_Py_SetPythonHome(pPy2Home);
	}
	else
	{
		Log("Python home not provided");
	}

	/*
	If the Python Home has been incorrectly then Py_Initialize() will fail to find the 'site' package.
	This is a fatal error and will kill the entire Python3 process.
	To avoid this we set a flag to not load the site module when we initialize. (This is the equivalent of starting Python with the '-s' flag).
	By doing this we can manually load the site module after the runtime has been initialized.
	If this then fails then it raises a Python exception but will not end the process.
	*/
	*PY2_Py_NoSiteFlag = 1;

	PY2_Py_Initialize();

	return PY3_Py_BuildValue("");
}

/**
* Execute a Python string in the Python2 interpreter
*
* @param pSelf Unused
* @param pArgs Argument tuple containing the command to execute, and the execution mode
**/
static PyObject* Py2_Run(PyObject* pSelf, PyObject* pArgs)
{
	Log("Enter C Py2_Exec\n");

	char* pCommand;
	int start;
	int execMode;
	PyObject* pRunResult;

	if (!Py2IsInitialized(NULL, NULL))
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Interpreter is not Initialized.");
		return NULL;
	}

	if (!PY3_PyArg_ParseTuple(pArgs, "si", &pCommand, &execMode))
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}

	if (execMode == EXEC_MODE_EVAL)
	{
		start = Py_eval_input;
		Log("Command to eval: '%s'\n", pCommand);

	}
	else if (execMode == EXEC_MODE_EXEC)
	{
		start = Py_file_input;
		Log("Command to exec: '%s'\n", pCommand);
	}
	else
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to parse exec mode.");
		return NULL;
	}

	pRunResult = RunString(pCommand, start);
	if (!pRunResult)
	{
		// RunString() should have cleaned up and set/printed the error value
		return NULL;
	}

	return pRunResult;
}

/**
* Finalize the Python2 Interpreter. This is done by invoking Py_Finalize() and closing the dynamically loaded SO file
*
* @param pSelf Unused
* @param pArgs Unused
**/
static PyObject* Finalize(PyObject* pSelf, PyObject* pArgs)
{
	Log("Enter C Finalize\n");
	if (!Py2IsInitialized())
	{
		Log("Interpreter is not Initialized.\n");
		PY3_PyErr_WarnEx(PyExc_Warning, "Interpreter is not Initialized.", 1);
		return PY3_Py_BuildValue("");
	}

	Log("Calling Py_Finalize()\n");
	PY2_Py_Finalize();

	if (!ClosePy27())
	{
		// ClosePython27() will have set the error
		Log("Failed to close the Python2 binary\n");
		return NULL;
	}

	return PY3_Py_BuildValue("");
}


/**
* Start of the Python Extension boilerplate code
**/
static PyMethodDef mainMethods[] = {
	{"Initialize", Initialize, METH_VARARGS, "Initialize the Python2 Interpreter"},
	{"IsInitialized", IsInitialized, METH_VARARGS, "Check if the Python2 Interpreter is initialized"},
	{"Py2_Run", Py2_Run, METH_VARARGS, "Execute or evaluate a string in the Python2 interpreter"},
	{"Finalize", Finalize, METH_VARARGS, "Close the Python2 interpreter"},
	{NULL, NULL, 0, NULL}
};

static PyModuleDef Py2Em = {
	PyModuleDef_HEAD_INIT,
	"Py2Em", "Run code in a Python2 interpreter",
	-1,
	mainMethods
};

PyMODINIT_FUNC PyInit__py2_em(void) {
	return PyModule_Create(&Py2Em);
}

/**
* End of the Python Extension boilerplate code
**/