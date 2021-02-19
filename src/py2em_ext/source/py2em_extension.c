#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Python.h>

#include "marshal_datatypes.h"
#include "marshal_utils.h"
#include "py2em_extension.h"


static PyObject* IsInitialized(PyObject* self, PyObject* args)
{
	return Py_BuildValue("O", Py2IsInitialized() ? Py_True : Py_False);
}

/**
* Invoke PyRun_SimpleString() from the loaded libpython SO file
*
* @param command Python command to execute
**/
PyObject *RunString(const char *command, int start)
{
	PyRun_StringFunc pPyRunString;
	PyImport_AddModuleFunc pPyImportAddModule;
	PyModule_GetDictFunc pPyModuleGetDict;

	pPyRunString = (PyRun_StringFunc)GetPy2Func("PyRun_String");
	pPyModuleGetDict = (PyModule_GetDictFunc)GetPy2Func("PyModule_GetDict");
	pPyImportAddModule = (PyImport_AddModuleFunc)GetPy2Func("PyImport_AddModule");


	if (!pPyRunString || !pPyImportAddModule || !pPyModuleGetDict)
	{
		return NULL;
	}

	PyObject *pMainMod;
	pMainMod = pPyImportAddModule("__main__");
	if (!pMainMod)
	{
		printf("TODO - PyImport_AddModule FAILED\n");
		return NULL;
	}

	PyObject *pLocalsGlobals;
	pLocalsGlobals = pPyModuleGetDict(pMainMod);
	if (!pLocalsGlobals)
	{
		printf("TODO - PyModule_GetDict FAILED\n");
		return NULL;
	}

	PyObject* pRunStrRes = pPyRunString(command, start, pLocalsGlobals, pLocalsGlobals);
	printf("Finished PyRun_String\n");

	// TODO - dispose of dictionaries
	// TODO - dispose of pRunStrRes

	if (!pRunStrRes)
	{
		PyErr_PrintFunc pPyErrPrintFunc;
		pPyErrPrintFunc = (PyErr_PrintFunc)GetPy2Func("PyErr_Print");
		if (!pPyErrPrintFunc)
		{
			return NULL;
		}
		pPyErrPrintFunc();
		return NULL;
	}

	if (start == Py_eval_input)
	{
		return MarshalObjectPy2ToPy3(pRunStrRes);
	}
	else
	{
		printf("Returning NULL\n");
		return NULL;
	}
}

/**
* Initialize the Python2 Interpreter. This is done by dynamically loading the shared library and invoking Py_Initialize()
*
* @param self Unused
* @param args Argument dictionary
**/
static PyObject* Initialize(PyObject* self, PyObject* args)
{
    char *so_filepath;
	if (!PyArg_ParseTuple(args, "s", &so_filepath))
	{
		PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}
	;
	if (!LoadPython27(so_filepath))
	{
	    // LoadPython27() should have set the error
	    return NULL;
	}

	PyInitializeFunc pyInitializePtr;
	pyInitializePtr = (PyInitializeFunc)GetPy2Func("Py_Initialize");
	if (!pyInitializePtr)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}

	pyInitializePtr();
	
	return Py_BuildValue("");
}

/**
* Execute a Python string in the Python2 interpreter
*
* @param self Unused
* @param args Argument tuple containing the command to execute
**/
static PyObject* Py2_Exec(PyObject* self, PyObject* args)
{
	char *command;

	if (!Py2IsInitialized(NULL, NULL))
	{
		PyErr_SetString(PyExc_RuntimeError, "Interpreter is not Initialized.");
		return NULL;
	}

	if (!PyArg_ParseTuple(args, "s", &command))
	{
		PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}

	RunString(command, Py_file_input);

	return Py_BuildValue("");
}

/**
* Evaluate a Python string in the Python2 interpreter
*
* @param self Unused
* @param args Argument tuple containing the command to evaluate
**/
static PyObject* Py2_Eval(PyObject* self, PyObject* args)
{
	char *command;

	if (!Py2IsInitialized())
	{
		PyErr_SetString(PyExc_RuntimeError, "Interpreter is not Initialized.");
		return NULL;
	}

	if (!PyArg_ParseTuple(args, "s", &command))
	{
		PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}

	PyObject * res = RunString(command, Py_eval_input);

	return res;
}

/**
* Finalize the Python2 Interpreter. This is done by invoking Py_Finalize() and closing the dynamically loaded SO file
*
* @param self Unused
* @param args Unused
**/
static PyObject* Finalize(PyObject* self, PyObject* args)
{
	if (!Py2IsInitialized())
	{
		PyErr_SetString(PyExc_RuntimeError, "Interpreter is not Initialized.");
		return NULL;
	}

	PyFinalizeFunc pyFinalizePtr;
	pyFinalizePtr = (PyFinalizeFunc)GetPy2Func("Py_Finalize");

	if (!pyFinalizePtr)
	{
		// GetPy2Func() will have set the error
		return NULL;
	}

	pyFinalizePtr();	

	if (!ClosePython27())
	{
		// ClosePython27() will have set the error
		return NULL;
	}

	return Py_BuildValue("");
}




/**
* Start of the Python Extension boilerplate code
**/
static PyMethodDef mainMethods[] = {
	{"Initialize", Initialize, METH_VARARGS, "Initialize the Python2.7 Interpreter"},
	{"IsInitialized", IsInitialized, METH_VARARGS, "Check if the Python2.7 Interpreter is initialized"},
	{"Py2_Exec", Py2_Exec, METH_VARARGS, "Execute a string in the Python2.7 interpreter"},
	{"Py2_Eval", Py2_Eval, METH_VARARGS, "Evaluate a string in the Python2.7 interpreter"},
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