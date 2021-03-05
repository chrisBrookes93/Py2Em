#ifndef MARSHAL_DATATYPES_h__
#define MARSHAL_DATATYPES_h__

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "marshal_utils.h"
#include "logging.h"
#include "py2_decref.h"

#define STR_MATCH 0

PyObject* MarshalLongPy2toPy3(PyObject* py2List);
PyObject* MarshalFloatPy2ToPy3(PyObject* py2Obj);
PyObject* MarshalComplexPy2ToPy3(PyObject* py2Obj);
PyObject* MarshalListPy2ToPy3(PyObject* py2List);
PyObject* MarshalSetPy2ToPy3(PyObject* py2Obj);
PyObject* MarshalDictPy2ToPy3(PyObject* py2Obj);
PyObject* MarshalTuplePy2ToPy3(PyObject* py2Obj);
PyObject* MarshalBoolPy2ToPy3(PyObject* py2Obj);
PyObject* MarshalStringPy2ToPy3(PyObject* py2Obj);
PyObject* MarshalObjectPy2ToPy3(PyObject* py2Obj);

#endif // MARSHAL_DATATYPES_h__
