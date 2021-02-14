#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <Python.h>


typedef void (*PyInitializeFunc)(void);
typedef void (*PyFinalizeFunc)(void);
typedef int (*PyRun_SimpleStringFunc)(const char *);
typedef PyObject* (*PyRun_StringFlagsFunc)(const char *, int, PyObject*, PyObject*, PyCompilerFlags *);


void *glob_py_handle;



void *LoadPython27(const char *pFilePath)
{
	printf("Loading Python Binary...");
	// This has to be DEEPBIND because within our Python3 extension there are already functions like Py_Initialize(). 
	// In order to make sure we call the functions in the Py2 .so file, we need to specify RTLD_DEEPBIND to place the
	// lookup scope ahead of the globals
	void *py_handle;
	py_handle = dlopen(pFilePath, RTLD_NOW | RTLD_DEEPBIND);
	if (!py_handle)
	{
		PyErr_SetString(PyExc_RuntimeError, dlerror());
		fputs(dlerror(), stderr);
		return NULL;
	}
	printf("Complete\n");
	return py_handle;
}


void InitializePython(void *py_handle)
{
	printf("Initializing Python...\n");
	PyInitializeFunc pyInitializePtr;
	pyInitializePtr = (PyInitializeFunc)dlsym(py_handle, "Py_Initialize");
	char* error;
	error = dlerror();

	if (error != NULL || !pyInitializePtr)
	{
		printf("ERROR: %s \n", error);
	}
	pyInitializePtr();

	if (error != NULL || !pyInitializePtr)
	{
		printf("ERROR: %s \n", error);
	}
	printf("Complete\n");
}

void FinalizePython(void *py_handle)
{
	PyFinalizeFunc pyFinalizePtr;
	pyFinalizePtr = (PyFinalizeFunc)dlsym(py_handle, "Py_Finalize");
	pyFinalizePtr();
}


void RunString(void *py_handle, char *command)
{
	PyRun_SimpleStringFunc pyRunSimpleStringPtr;
	pyRunSimpleStringPtr = (PyRun_SimpleStringFunc)dlsym(py_handle, "PyRun_SimpleString");
	pyRunSimpleStringPtr(command);
}

PyObject* RunStringFlags(void *py_handle, char *command, int start, PyObject *globals, PyObject *locals, PyCompilerFlags *flags)
{
	PyRun_StringFlagsFunc pyRunStringFlagsPtr;
	pyRunStringFlagsPtr = (PyRun_StringFlagsFunc)dlsym(py_handle, "PyRun_StringFlags");
	return pyRunStringFlagsPtr(command, start, globals, locals, flags);
	//return NULL;
}

static PyObject* Init(PyObject* self, PyObject* args)
{
	glob_py_handle = LoadPython27("libpython2.7.so");
	InitializePython(glob_py_handle);
	return Py_BuildValue("");
}

static PyObject* Py2_Exec(PyObject* self, PyObject* args)
{
	char *command;
	if (!glob_py_handle)
	{
		PyErr_SetString(PyExc_RuntimeError, "Python Interpreter is not Initialized. You need to call Init() first.");
		return NULL;
	}

	if (!PyArg_ParseTuple(args, "s", &command))
	{
		PyErr_SetString(PyExc_RuntimeError, "Failed to parse input args.");
		return NULL;
	}
	RunString(glob_py_handle, command);
	return Py_BuildValue("");
}

static PyObject* Finalize(PyObject* self, PyObject* args)
{
	if (!glob_py_handle)
	{
		PyErr_SetString(PyExc_RuntimeError, "Python Interpreter is not Initialized.");
		return NULL;
	}

	FinalizePython(glob_py_handle);
	dlclose(glob_py_handle);
	glob_py_handle = NULL;
	return Py_BuildValue("");
}


static PyObject* runstr(PyObject* self, PyObject* args)
{
	printf("Native entrypoint\n");

	char *command;
	if (!PyArg_ParseTuple(args, "s", &command))
	{
		return NULL;
	}
	printf("Command to exec is: '%s'\n", command);

	// Dynamically load the python2 shared library
	void *py_handle;
	py_handle = LoadPython27("libpython2.7.so");

	// Initialize the Python2 Interpreter
	InitializePython(py_handle);


	RunString(py_handle, "i = 1337");
	RunString(py_handle, "print(i)");
	RunString(py_handle, command);

	//PyObject* ret;
	//PyObject *globals = Py_BuildValue("{}");
	//PyObject *locals = Py_BuildValue("{}");
	//ret = RunStringFlags(py_handle, "100+100", Py_eval_input, NULL, NULL, NULL);

	//return ret;
	return Py_BuildValue("");
	//return Py_BuildValue("i", result);
}

static PyMethodDef mainMethods[] = {
	{"Init", Init, METH_VARARGS, "Initialize the Python2.7 Interpreter"},
	{"Py2_Exec", Py2_Exec, METH_VARARGS, "Execute a string in the Python2.7 interpreter"},
	{"Finalize", Finalize, METH_VARARGS, "Close the Python2.7 interpreter"},
	{"rs", runstr, METH_VARARGS, "Run code in a Python2.7 Interpreter"},
	{NULL, NULL, 0, NULL}
};

static PyModuleDef Py2Em = {
	PyModuleDef_HEAD_INIT,
	"Py2Em", "Run code in a Python2.7 interpreter",
	-1,
	mainMethods
};

PyMODINIT_FUNC PyInit_Py2Em(void) {
	return PyModule_Create(&Py2Em);
}

/*
int main(void)
{
	void *py_handle;
	//py_handle = LoadPython27("libpython2.7.so");
	py_handle = LoadPython27("/tmp/python/lib/libpython2.7.so");

	SetPythonHome(py_handle, "/tmp/python/lib/python2.7");
	printf("A\n");

	//SetProgramName(py_handle, "Hello Python2");
	(void)IsInitialized(py_handle);
	printf("B\n");

	InitThreads(py_handle);
	printf("Calliing Init python shim\n");

	InitializePython(py_handle);
	printf("Outside Init python shim\n");
	(void)IsInitialized(py_handle);

	RunString(py_handle, "");
}*/