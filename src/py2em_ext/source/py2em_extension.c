#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Python.h>

#include "marshal_datatypes.h"
#include "marshal_utils.h"


/**
* Typedefs for the functions invoked in the libpython2.7.so file
**/
typedef void (*PyInitializeFunc)(void);
typedef void (*PyFinalizeFunc)(void);
typedef void (*PyErr_PrintFunc)(void);
typedef PyObject* (*PyDict_NewFunc)(void);

// For converting Py2 datatypes -> C datatypes




typedef PyObject* (*PyRun_StringFunc)(const char *, int start, PyObject *globals, PyObject *locals);

/**
* Handle to the libpython2.7.so file
**/
void *globPyHandle;




/**
* Invoke Py_Initialize() from the loaded libpython SO file
*
* @param pyHandle Handle to the libpython SO file
**/
bool InitializePython(void *pyHandle)
{
    bool retVal = false;

	PyInitializeFunc pyInitializePtr;
	pyInitializePtr = (PyInitializeFunc)dlsym(pyHandle, "Py_Initialize");
	char* error;
	error = dlerror();

	if (error != NULL || !pyInitializePtr)
	{
		PyErr_SetString(PyExc_RuntimeError, error);
		return retVal;
	}
	pyInitializePtr();

    error = dlerror();
	if (error != NULL)
	{
		PyErr_SetString(PyExc_RuntimeError, error);
		return retVal;
	}
	retVal = true;
	return retVal;
}

/**
* Invoke Py_Finalize() from the loaded libpython SO file
*
* @param pyHandle Handle to the libpython SO file
**/
void FinalizePython(void *pyHandle)
{
	PyFinalizeFunc pyFinalizePtr;
	pyFinalizePtr = (PyFinalizeFunc)dlsym(pyHandle, "Py_Finalize");
	pyFinalizePtr();
}





/**
* Invoke PyRun_SimpleString() from the loaded libpython SO file
*
* @param pyHandle Handle to the libpython SO file
* @param command Python command to execute
**/
PyObject *RunString(void *pyHandle, const char *command, int start, PyObject *globals, PyObject *locals)
{
	//PyObject *main_module = PyImport_AddModule("__main__");
	//PyObject *global_dict = PyModule_GetDict(main_module);
	PyDict_NewFunc pyDictNewptr;
	pyDictNewptr = (PyDict_NewFunc)dlsym(pyHandle, "PyDict_New");
	PyObject* globs = pyDictNewptr();
	PyObject* locs = pyDictNewptr();


	PyRun_StringFunc pyRunStringPtr;
	pyRunStringPtr = (PyRun_StringFunc)dlsym(pyHandle, "PyRun_String");
	PyObject* res = pyRunStringPtr(command, start, globs, locs);

	return MarshalObjectPy2ToPy3(pyHandle, res);
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

	globPyHandle = LoadPython27(so_filepath);
	if (!globPyHandle)
	{
	    // LoadPython27() should have set the error
	    return NULL;
	}
	if (!InitializePython(globPyHandle))
	{
		// InitializePython() should have set the error
	    return NULL;
	}
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
	if (!globPyHandle)
	{
		PyErr_SetString(PyExc_RuntimeError, "Python Interpreter is not Initialized.");
		return NULL;
	}

	if (!PyArg_ParseTuple(args, "s", &command))
	{
		PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}

	PyObject *global_dict = PyDict_New();
	PyObject *local_dict = PyDict_New();
	PyObject * res = RunString(globPyHandle, command, Py_eval_input, global_dict, local_dict);
	//printf("%d", res);

	//Py_DECREF(global_dict);
	//Py_DECREF(local_dict);

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
	if (!globPyHandle)
	{
		PyErr_SetString(PyExc_RuntimeError, "Python Interpreter is not Initialized.");
		return NULL;
	}

	FinalizePython(globPyHandle);
	dlclose(globPyHandle);
	globPyHandle = NULL;
	return Py_BuildValue("");
}

/**
* Start of the Python Extension boilerplate code
**/
static PyMethodDef mainMethods[] = {
	{"Initialize", Initialize, METH_VARARGS, "Initialize the Python2.7 Interpreter"},
	{"Py2_Exec", Py2_Exec, METH_VARARGS, "Execute a string in the Python2.7 interpreter"},
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