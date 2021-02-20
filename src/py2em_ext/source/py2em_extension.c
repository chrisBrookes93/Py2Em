#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Python.h>

#include "marshal_datatypes.h"
#include "marshal_utils.h"
#include "py2em_extension.h"


static PyObject* IsInitialized(PyObject* self, PyObject* args)
{
	Log("Enter C IsInitialized\n");
	return PY3_Py_BuildValue("O", Py2IsInitialized() ? Py_True : Py_False);
}

/**
* Invoke PyRun_SimpleString() from the loaded libpython SO file
*
* @param command Python command to execute
**/
PyObject *RunString(const char *command, int start)
{
	Log("Enter C RunString\n");
	Log("Importing __main__ to get the locals/globals...");
	PyObject *pMainMod;
	pMainMod = PY2_PyImport_AddModule("__main__");
	if (!pMainMod)
	{
		Log("TODO - PyImport_AddModule FAILED\n");
		return NULL;
	}
	Log("Success\nObtaining the variable dictionary...");

	PyObject *pLocalsGlobals;
	pLocalsGlobals = PY2_PyModule_GetDict(pMainMod);
	if (!pLocalsGlobals)
	{
		Log("TODO - PyModule_GetDict FAILED\n");
		return NULL;
	}
	Log("Success.\nCalling PyRun_String()...");

	PyObject* pRunStrRes = PY2_PyRun_String(command, start, pLocalsGlobals, pLocalsGlobals);

	// TODO - dispose of dictionaries
	// TODO - dispose of pRunStrRes

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
		return MarshalObjectPy2ToPy3(pRunStrRes);
	}
	else
	{
		Log("We are not evaluating, so returning empty value (None)\n");
		return PY3_Py_BuildValue("");
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
	Log("Enter C Initialize()\n");
    if (Py2IsInitialized())
	{
		PY3_PyErr_WarnEx(PyExc_Warning, "Interpreter is already Initialized.", 1);
		PY3_Py_BuildValue("");
	}

    char *so_filepath;
	if (!PY3_PyArg_ParseTuple(args, "s", &so_filepath))
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}
	;
	if (!LoadPython2AndInitFuncs(so_filepath))
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
* @param self Unused
* @param args Argument tuple containing the command to execute
**/
static PyObject* Py2_Exec(PyObject* self, PyObject* args)
{
	Log("Enter C Py2_Exec\n");

	char *command;

	if (!Py2IsInitialized(NULL, NULL))
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Interpreter is not Initialized.");
		return NULL;
	}

	if (!PY3_PyArg_ParseTuple(args, "s", &command))
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}

	if (!RunString(command, Py_file_input))
	{
		return NULL;
	}

	return PY3_Py_BuildValue("");
}

/**
* Evaluate a Python string in the Python2 interpreter
*
* @param self Unused
* @param args Argument tuple containing the command to evaluate
**/
static PyObject* Py2_Eval(PyObject* self, PyObject* args)
{
	Log("Enter C Py2_Eval\n");

	char *command;

	if (!Py2IsInitialized())
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Interpreter is not Initialized.");
		return NULL;
	}

	if (!PyArg_ParseTuple(args, "s", &command))
	{
		PY3_PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
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