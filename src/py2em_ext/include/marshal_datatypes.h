#ifndef MARSHAL_DATATYPES_h__
#define MARSHAL_DATATYPES_h__

#include <Python.h>
#include "marshal_utils.h"
#include "logging.h"

PyObject *MarshalListPy2ToPy3(PyObject *py2List);
PyObject *MarshalLongPy2toPy3(PyObject *py2List);
PyObject *MarshalStringPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalFloatPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalComplexPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalObjectPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalBoolPy2ToPy3(PyObject *py2Obj);
PyObject *MarshalSetPy2ToPy3(PyObject *py2Obj);

#endif // MARSHAL_DATATYPES_h__