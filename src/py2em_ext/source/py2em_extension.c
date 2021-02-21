#include "py2em_extension.h"

/**
* Returns a bool indicating whether the Python2 emulator is initialized
*
* @param pSelf Unused
* @param pArgs Unused
* @return Python-encoded boolean indicating initialized state
**/
static PyObject* IsInitialized(PyObject *pSelf, PyObject *pArgs)
{
	Log("Enter C IsInitialized\n");
	return PY3_Py_BuildValue("O", Py2IsInitialized() ? Py_True : Py_False);
}

/**
* Invoke PyRun_String() from the loaded libpython SO file.
* This code is based upon Python/pythonrun.c#PyRun_SimpleStringFlags(). Note that PyRun_SimpleStringFlags() 
* does not Py_DECREF the pointers returned by PyImport_AddModule("__main__") and PyModule_GetDict(), so we don't either.
*
* @param pCommand Python command to execute
* @param start Evaluation mode (e.g. Py_eval_input)
* @return Python-encoded result
**/
PyObject *RunString(const char *pCommand, int start)
{
	Log("Enter C RunString\n");
	PyObject *pRetVal;
	PyObject *pMainMod;
	PyObject *pLocalsGlobals;
	PyObject *pRunStrRes;

	Log("Importing __main__ to get the locals/globals...");

	// Examples in Python/pythonrun.c#PyRun_SimpleStringFlags() does not DECREF this object
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
		Log("Failed. Printing Error:\n");
		PY2_PyErr_Print();
		return PY3_Py_BuildValue("");
	}
	Log("Success.\n");

	if (start == Py_eval_input)
	{
		Log("Marshalling return data...\n");
		pRetVal = MarshalObjectPy2ToPy3(pRunStrRes);
		PY2_Py_DECREF(pRunStrRes);
		return pRetVal;
	}
	else
	{
		PY2_Py_DECREF(pRunStrRes);
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
static PyObject* Initialize(PyObject *pSelf, PyObject *pArgs)
{
    Log("Enter C Initialize()\n");

    char *pSo_filepath;

    if (Py2IsInitialized())
	{
		PY3_PyErr_WarnEx(PyExc_Warning, "Interpreter is already Initialized.", 1);
		return PY3_Py_BuildValue("");
	}

	if (!PY3_PyArg_ParseTuple(pArgs, "s", &pSo_filepath))
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}
	
	if (!LoadPython2AndInitFuncs(pSo_filepath))
	{
	    // LoadPython27() should have set the error
	    return NULL;
	}

	PY2_Py_Initialize();

	return PY3_Py_BuildValue("");
}

/**
* Execute a Python string in the Python2 interpreter
*
* @param pSelf Unused
* @param pArgs Argument tuple containing the command to execute, and the execution mode
**/
static PyObject* Py2_Run(PyObject *pSelf, PyObject *pArgs)
{
	Log("Enter C Py2_Exec\n");

	char *pCommand;
	int start;
	int execMode;
	PyObject *pRunResult;

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
		Log("Command to eval: %s\n", pCommand);

	}
	else if (execMode == EXEC_MODE_EXEC)
	{
		start = Py_file_input;
		Log("Command to exec: %s\n", pCommand);
	}
	else
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
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
static PyObject* Finalize(PyObject *pSelf, PyObject *pArgs)
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
		
	if (!ClosePython27())
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
	{"Initialize", Initialize, METH_VARARGS, "Initialize the Python2.7 Interpreter"},
	{"IsInitialized", IsInitialized, METH_VARARGS, "Check if the Python2.7 Interpreter is initialized"},
	{"Py2_Run", Py2_Run, METH_VARARGS, "Execute or evaluate a string in the Python2.7 interpreter"},
	{"Finalize", Finalize, METH_VARARGS, "Close the Python2.7 interpreter"},
	{NULL, NULL, 0, NULL}
};

static PyModuleDef Py2Em = {
	PyModuleDef_HEAD_INIT,
	"Py2Em", "Run code in a Python2.7 interpreter",
	-1,
	mainMethods
};

PyMODINIT_FUNC PyInit__py2_em(void) {
	return PyModule_Create(&Py2Em);
}

/**
* End of the Python Extension boilerplate code
**/