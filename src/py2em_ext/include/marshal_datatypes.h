#ifndef MARSHAL_DATATYPES_h__
#define MARSHAL_DATATYPES_h__

#include <Python.h>
#include "marshal_utils.h"

typedef char* (*PyString_AsStringFunc)(PyObject *);
typedef long (*PyLongAsLongFunc)(PyObject *);

PyObject *MarshalListPy2ToPy3(PyObject *py2List);
PyObject *MarshalLongPy2toPy3(PyObject *py2List);
PyObject *MarshalStringPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalObjectPy2ToPy3(PyObject *py2Obj);

#endif // MARSHAL_DATATYPES_h__