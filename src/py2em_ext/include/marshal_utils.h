#ifndef MARSHAL_UTILS_h__
#define MARSHAL_UTILS_h__

#define PY_SSIZE_T_CLEAN

#include <stdbool.h>
#include <Python.h>
#include "logging.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif


#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0
#endif

#define ERROR_OVERFLOW 1
#define ERROR_UNDERFLOW -1

/**
* Typedefs for the Python2 functions used
**/
typedef void 				(*Py_Initialize_t)					(void);
typedef void 				(*Py_Finalize_t)					(void);
typedef PyObject*			(*PyObject_GetIter_t)				(PyObject*);
typedef PyObject*			(*PyIter_Next_t)					(PyObject*);
typedef Py_ssize_t			(*PyList_Size_t)					(PyObject*);
typedef long  				(*PyLong_AsLongAndOverflow_t)		(PyObject*, int*);
typedef long long 			(*PyLong_AsLongLongAndOverflow_t)	(PyObject*, int*);
typedef unsigned long long 	(*PyLong_AsUnsignedLongLong_t)		(PyObject*);
typedef int   				(*PyObject_IsTrue_t)   				(PyObject*);
typedef double				(*PyFloat_AsDouble_t)				(PyObject*);
typedef double 				(*PyComplex_RealAsDouble_t)			(PyObject*);
typedef double 				(*PyComplex_ImagAsDouble_t)			(PyObject*);
typedef char* 				(*PyString_AsString_t) 				(PyObject*);
typedef PyObject*			(*PyObject_Str_t)					(PyObject*);
typedef PyObject* 			(*PyRun_String_t)					(const char*, int, PyObject*, PyObject*);
typedef PyObject* 			(*PyImport_AddModule_t)				(const char*);
typedef PyObject* 			(*PyModule_GetDict_t)				(PyObject*);
typedef void 				(*PyErr_Print_t)					(void);
typedef int 				(*PyDict_Next_t)					(PyObject*, Py_ssize_t*, PyObject**, PyObject**);
typedef Py_ssize_t			(*PyTuple_Size_t)					(PyObject*);
typedef void                (*Py_SetPythonHome_t)               (const char*);
typedef void                (*PyErr_Fetch_t)                    (PyObject**, PyObject**, PyObject**);
typedef int*                Py_NoSiteFlag_t;

/*
* Aliases for the Python3 functions. As most Python functions exist in 2 & 3.
* Using these makes it easier to distinguish in the code that we're calling the Python3 version from Python.h, rather
* than the Python2 version from the loaded binary
*/
#define PY3_PyList_New PyList_New 
#define PY3_PyList_SetItem PyList_SetItem
#define PY3_PySet_New PySet_New
#define PY3_PySet_Add PySet_Add
#define PY3_Py_XDECREF Py_XDECREF
#define PY3_Py_BuildValue Py_BuildValue
#define PY3_PyErr_SetString PyErr_SetString
#define PY3_PyComplex_FromDoubles PyComplex_FromDoubles
#define PY3_PyArg_ParseTuple PyArg_ParseTuple
#define PY3_PyErr_WarnEx PyErr_WarnEx
#define PY3_PyDict_New PyDict_New
#define PY3_PyDict_SetItem PyDict_SetItem
#define PY3_PyTuple_New PyTuple_New
#define PY3_PyTuple_SetItem PyTuple_SetItem
#define PY3_PyErr_Format PyErr_Format

bool Py2IsInitialized();
bool LoadPy2AndResolveSymbols(const char *pFilePath);
bool InitializePy2Symbols();
void UninitializePy2Symbols();
void *GetPy2Symbol(const char *pSymbolName);
bool ClosePy27();


/*
* Handle to the Python2 binary
*/
extern void *pGlobPyHandle;

/*
* Python2 function pointers
*/
extern PyObject_GetIter_t             PY2_PyObject_GetIter;
extern PyIter_Next_t                  PY2_PyIter_Next;
extern PyList_Size_t                  PY2_PyList_Size;
extern Py_Initialize_t                PY2_Py_Initialize;
extern Py_Finalize_t                  PY2_Py_Finalize;
extern PyLong_AsLongAndOverflow_t     PY2_PyLong_AsLongAndOverflow;
extern PyLong_AsLongLongAndOverflow_t PY2_PyLong_AsLongLongAndOverflow;
extern PyLong_AsUnsignedLongLong_t    PY2_PyLong_AsUnsignedLongLong;
extern PyObject_IsTrue_t              PY2_PyObject_IsTrue;
extern PyFloat_AsDouble_t             PY2_PyFloat_AsDouble;
extern PyComplex_RealAsDouble_t       PY2_PyComplex_RealAsDouble;
extern PyComplex_ImagAsDouble_t       PY2_PyComplex_ImagAsDouble;
extern PyString_AsString_t            PY2_PyString_AsString;
extern PyObject_Str_t                 PY2_PyObject_Str;
extern PyRun_String_t                 PY2_PyRun_String;
extern PyModule_GetDict_t             PY2_PyModule_GetDict;
extern PyImport_AddModule_t           PY2_PyImport_AddModule;
extern PyErr_Print_t                  PY2_PyErr_Print;
extern PyDict_Next_t                  PY2_PyDict_Next;
extern PyTuple_Size_t                 PY2_PyTuple_Size;
extern Py_SetPythonHome_t             PY2_Py_SetPythonHome;
extern PyErr_Fetch_t                  PY2_PyErr_Fetch;
extern Py_NoSiteFlag_t                PY2_Py_NoSiteFlag;

#endif // MARSHAL_UTILS_h__
