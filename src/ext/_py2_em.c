#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <Python.h>

/**
* Typedefs for the functions invoked in the libpython2.7.so file
**/
typedef void (*PyInitializeFunc)(void);
typedef void (*PyFinalizeFunc)(void);
typedef int (*PyRun_SimpleStringFunc)(const char *);

/**
* Handle to the libpython2.7.so file
**/
void *globPyHandle;


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
void RunString(void *pyHandle, char *command)
{
	PyRun_SimpleStringFunc pyRunSimpleStringPtr;
	pyRunSimpleStringPtr = (PyRun_SimpleStringFunc)dlsym(pyHandle, "PyRun_SimpleString");
	pyRunSimpleStringPtr(command);
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
	InitializePython(globPyHandle);
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
		PyErr_SetString(PyExc_RuntimeError, "Python Interpreter is not initialized.");
		return NULL;
	}

	if (!PyArg_ParseTuple(args, "s", &command))
	{
		PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}

	RunString(globPyHandle, command);
	return Py_BuildValue("");
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