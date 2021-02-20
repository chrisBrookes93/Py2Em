#ifndef MARSHAL_UTILS_h__
#define MARSHAL_UTILS_h__

#include <dlfcn.h>
#include <Python.h>
#include <stdbool.h>
#include "logging.h"

typedef void 				(*Py_Initialize_t)					(void);
typedef void 				(*Py_Finalize_t)					(void);
typedef PyObject*			(*PyObject_GetIter_t)				(PyObject *);
typedef PyObject*			(*PyIter_Next_t)					(PyObject *);
typedef Py_ssize_t			(*PyList_Size_t)					(PyObject *);
typedef void				(*Py_XDECREF_t)						(PyObject *);
typedef long  				(*PyLong_AsLongAndOverflow_t)		(PyObject *, int*);
typedef long long 			(*PyLong_AsLongLongAndOverflow_t)	(PyObject *, int*);
typedef unsigned long long 	(*PyLong_AsUnsignedLongLong_t)		(PyObject *);
typedef int   				(*PyObject_IsTrue_t)   				(PyObject *);
typedef double				(*PyFloat_AsDouble_t)				(PyObject *);
typedef double 				(*PyComplex_RealAsDouble_t)			(PyObject *);
typedef double 				(*PyComplex_ImagAsDouble_t)			(PyObject *);
typedef char* 				(*PyString_AsString_t) 				(PyObject *);
typedef PyObject*			(*PyObject_Str_t)					(PyObject *);
typedef PyObject* 			(*PyRun_String_t)					(const char *, int start, PyObject *globals, PyObject *locals);
typedef PyObject* 			(*PyImport_AddModule_t)				(const char *name);
typedef PyObject* 			(*PyModule_GetDict_t)				(PyObject *module);
typedef void 				(*PyErr_Print_t)					(void);

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

bool Py2IsInitialized();
bool LoadPython2AndInitFuncs(const char *pFilePath);
bool InitializeFunctionPointers();
void UninitializeFunctionPointers();
void *GetPy2Func(const char *pSymbolName);
bool ClosePython27();


/**
* Handle to the libpython2.7.so file
**/
void *pGlobPyHandle;


PyObject_GetIter_t 				PY2_PyObject_GetIter;
PyIter_Next_t 					PY2_PyIter_Next;
PyList_Size_t					PY2_PyList_Size;
Py_Initialize_t					PY2_Py_Initialize;
Py_Finalize_t					PY2_Py_Finalize;
PyLong_AsLongAndOverflow_t 		PY2_PyLong_AsLongAndOverflow;
PyLong_AsLongLongAndOverflow_t 	PY2_PyLong_AsLongLongAndOverflow;
PyLong_AsUnsignedLongLong_t 	PY2_PyLong_AsUnsignedLongLong;
PyObject_IsTrue_t				PY2_PyObject_IsTrue;
PyFloat_AsDouble_t				PY2_PyFloat_AsDouble;
PyComplex_RealAsDouble_t		PY2_PyComplex_RealAsDouble;
PyComplex_ImagAsDouble_t		PY2_PyComplex_ImagAsDouble;
PyString_AsString_t				PY2_PyString_AsString;
PyObject_Str_t 					PY2_PyObject_Str;
PyRun_String_t					PY2_PyRun_String;
PyModule_GetDict_t				PY2_PyModule_GetDict;
PyImport_AddModule_t			PY2_PyImport_AddModule;
PyErr_Print_t					PY2_PyErr_Print;


#endif // MARSHAL_UTILS_h__