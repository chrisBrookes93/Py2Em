#ifndef MARSHAL_DATATYPES_h__
#define MARSHAL_DATATYPES_h__

#include <Python.h>
#include "marshal_utils.h"
/*
typedef char* 				(*PyStringAsStringFunc) 		(PyObject *);
typedef long  				(*PyLongAsLongAndOverflowFunc)  (PyObject *, int*);
typedef double				(*PyFloatAsDoubleFunc)			(PyObject *);
typedef double 				(*PyComplexRealAsDoubleFunc)	(PyObject *);
typedef double 				(*PyComplexImagAsDoubleFunc)	(PyObject *);
typedef long long 			(*PyLongAsLongLongOverflowFunc)	(PyObject *, int*);
typedef unsigned long long 	(*PyLongAsUnsignedLongLongFunc)	(PyObject *);

typedef PyObject*			(*PyObject_GetIter_t)			(PyObject *);
typedef PyObject*			(*PyIter_Next_t)				(PyObject *);
typedef Py_ssize_t			(*PyList_Size_t)				(PyObject *);
typedef void				(*Py_XDECREF_t)					(PyObject *);

#define PY3_PyList_New PyList_New 
#define PY3_PyList_SetItem PyList_SetItem
#define PY3_PySet_New PySet_New
#define PY3_Py_XDECREF Py_XDECREF
#define PY3_PySet_Add PySet_Add
*/
PyObject *MarshalListPy2ToPy3(PyObject *py2List);
PyObject *MarshalLongPy2toPy3(PyObject *py2List);
PyObject *MarshalStringPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalFloatPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalComplexPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalObjectPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalBoolPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalSetPy2ToPy3(PyObject *py2Obj);



#endif // MARSHAL_DATATYPES_h__